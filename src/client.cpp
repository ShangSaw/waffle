#include <iostream>
#include "enet/enet.h"
#include "SDL3/SDL.h"
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "packets.h"

constexpr auto WINDOW_W = 1280;
constexpr auto WINDOW_H = 720;

// player related constants
constexpr auto MOVE_SPEED = 5.0f;
constexpr auto PLAYER_WH = 50.0f;

typedef struct Coordonnees {
    float x, y;
} Coordonnees;

typedef struct Joueur {
    SDL_FRect r;
    SDL_FRect dr;
} Joueur;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static Joueur player;
static SDL_FRect camera;

static std::unordered_map<int, Joueur> joueurs;

static std::vector<std::string> split_string(const std::string& str, char delim = '|') {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delim)) {
        if (!token.empty())
            tokens.push_back(token);
    }
    return tokens;
}

static std::string StringifyPosition(float x, float y) {
    std::string msg = std::to_string(x) + "|" + std::to_string(y);
    return msg;
}

static void SetupSDL() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "couldn't initialize SDL video module " << std::endl;

        exit(EXIT_FAILURE);
    }
    if (!SDL_CreateWindowAndRenderer("feneettttre", WINDOW_W, WINDOW_H, 0, &window, &renderer)) {
        std::cerr << "couldn't create a window and a renderer" << std::endl;
    }
}

static void WindowDraw() {
    SDL_SetRenderDrawColorFloat(renderer, 1.0f, 1.0f, 1.0f, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(renderer);


    // TODO: redo drawing

    SDL_SetRenderDrawColorFloat(renderer, 1.0f, 0.0f, 0.0f, SDL_ALPHA_OPAQUE_FLOAT);

    SDL_RenderFillRect(renderer, &player.dr);

    for (auto& [_, j] : joueurs) {
        SDL_RenderFillRect(renderer, &j.dr); // <-- CHANGED HERE
    }

    SDL_RenderPresent(renderer);

}

static Coordonnees ProcessKeypresses() {
    const bool* keystates = SDL_GetKeyboardState(0);
    Coordonnees dir{};

    if (keystates[SDL_SCANCODE_LEFT])  dir.x = -1;
    if (keystates[SDL_SCANCODE_RIGHT]) dir.x = 1;
    if (keystates[SDL_SCANCODE_UP])    dir.y = -1;
    if (keystates[SDL_SCANCODE_DOWN])  dir.y = 1;


    return dir;
}

static void UpdateCamera() {
    camera.x = player.r.x + player.r.w / 2 - camera.w / 2;
    camera.y = player.r.y + player.r.h / 2 - camera.h / 2;
}

static bool Update(bool& fullscreen) {

    SDL_Event event;
    Coordonnees dir{};

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            return false;
        case SDL_EVENT_KEY_UP:
            if (event.key.key == SDLK_RETURN) {
                fullscreen = !fullscreen;
                SDL_SetWindowFullscreen(window, fullscreen);
                int w, h;

                SDL_GetCurrentRenderOutputSize(renderer, &w, &h);
                std::cout << w << " " << h << std::endl;

                camera.w = (float)w;
                camera.h = (float)h;
            }

        }
    }

    
    dir = ProcessKeypresses();
    player.r.x += dir.x * MOVE_SPEED;
    player.r.y += dir.y * MOVE_SPEED;

    UpdateCamera();

    player.dr.x = player.r.x - camera.x;
    player.dr.y = player.r.y - camera.y;
    player.dr.w = player.r.w;
    player.dr.h = player.r.h;

    for (auto& [id, j] : joueurs) {
        j.dr.x = j.r.x - camera.x;
        j.dr.y = j.r.y - camera.y;
        j.dr.w = j.r.w;
        j.dr.h = j.r.h;
    }

    return true;
}

static void CleanSDL() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void SendPacket(ENetPeer* peer, const char* data) {
    ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
    enet_peer_send(peer, 0, packet);
}

void ParsePacket(ENetEvent* e) {
    std::string data(reinterpret_cast<char*>(e->packet->data));
    std::vector<std::string> tokens = split_string(data);
    int packet_type = std::stoi(tokens[0]);
    int id = std::stoi(tokens[1]);

    switch (packet_type) {
    case PacketTypes::PLAYER_CONNECT:
        joueurs[id] = { {0.0f, 0.0f, PLAYER_WH, PLAYER_WH}, {0.0f, 0.0f, PLAYER_WH, PLAYER_WH} }; // Added dr here
        break;
    case PacketTypes::PLAYER_DISCONNECT:
        joueurs.erase(id);
        break;
    case PacketTypes::UPDATE_POSITION: {
        float x = std::stof(tokens[2]);
        float y = std::stof(tokens[3]);
        joueurs[id].r.x = x;
        joueurs[id].r.y = y;
        break;
    }
    }
}

void MsgLoop(ENetHost* client) {
    ENetEvent event;
    while (enet_host_service(client, &event, 5) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_RECEIVE: {
            std::cout << "Paquet reçu : " << event.packet->data << std::endl;
            ParsePacket(&event);
            enet_packet_destroy(event.packet);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "Serveur déconnecté." << std::endl;
            break;
        default:
            break;
        }
    }
}

void runClient(const char* addr) {
    ENetHost* client = enet_host_create(nullptr, 1, 1, 0, 0);
    if (client == nullptr) {
        std::cerr << "Erreur : couldn't create client." << std::endl;
        exit(EXIT_FAILURE);
    }

    ENetAddress address;
    ENetEvent event;
    ENetPeer* peer;

    enet_address_set_host(&address, addr);
    address.port = 1234;

    peer = enet_host_connect(client, &address, 1, 0);
    if (peer == nullptr) {
        std::cerr << "[CLIENT] Connexion failed." << std::endl;
        exit(EXIT_FAILURE);
    }

    if (enet_host_service(client, &event, 5000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
        std::cout << "[CLIENT] Connected to server!" << std::endl;
    }
    else {
        enet_peer_reset(peer);
        std::cerr << "[CLIENT] Connexion refused." << std::endl;
        exit(EXIT_FAILURE);
    }

    // game loop start

    // FPS capping
    Uint32 start;
    const int FPS = 60;

    // Packet timer setup
    const int packet_send_rate = 2;
    int packet_send_timer = 0;

    std::string pos_string;
    bool running = true;


    bool fullscreen = false;
    while (running) {
        start = (Uint32)SDL_GetTicks();
        
        running = Update(fullscreen);
        WindowDraw();
        MsgLoop(client);

        if (1000 / FPS > SDL_GetTicks() - start) {
            SDL_Delay(1000 / FPS - ((Uint32)SDL_GetTicks() - start));
        }

        if (packet_send_timer > packet_send_rate) {
            pos_string = StringifyPosition(player.r.x, player.r.y);
            SendPacket(peer, pos_string.c_str());
            packet_send_timer = 0;
        }

        packet_send_timer++;


        // Vérifier l'état de la connexion
        if (peer->state == ENET_PEER_STATE_DISCONNECTED) {
            std::cerr << "Connexion perdue." << std::endl;
        }
    }
    // game loop end
    CleanSDL();



    enet_peer_disconnect(peer, 0);
    while (enet_host_service(client, &event, 3000) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_RECEIVE:
            enet_packet_destroy(event.packet);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            std::cout << "Client disconnected successfully." << std::endl;
            break;
        default:
            break;
        }
    }
    enet_host_destroy(client);
}

int main(int argc, char** argv) {
    if (enet_initialize() != 0) {
        std::cerr << "Error: enet not initialized correctly" << std::endl;
        return EXIT_FAILURE;
    }
    SetupSDL();

    player.r.w = PLAYER_WH;
    player.r.h = PLAYER_WH;
    player.r.x = 0;
    player.r.y = 0;

    camera.w = WINDOW_W;
    camera.h = WINDOW_H;
    camera.x = 0;
    camera.y = 0;

    runClient("127.0.0.1");
    enet_deinitialize();
    return EXIT_SUCCESS;
}
