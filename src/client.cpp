#include <iostream>
#include "enet/enet.h"
#include "SDL3/SDL.h"
#include <sstream>
#include <vector>
#include <string>
#include <unordered_map>
#include "packets.h"

#define WINDOW_W 640
#define WINDOW_H 480

// player related constants
#define MOVE_SPEED 5.0f
#define PLAYER_WH 50.0f

typedef struct Coordonnees {
    float x, y;
} Coordonnees;

typedef struct Joueur {
    SDL_FRect r;
} Joueur;

static SDL_Window* window = NULL;
static SDL_Renderer* renderer = NULL;
static SDL_FRect player;

static std::unordered_map<int, Joueur> joueurs;

std::vector<std::string> split_string(const std::string& str, char delim = '|') {
    std::vector<std::string> tokens;
    std::stringstream ss(str);
    std::string token;
    while (getline(ss, token, delim)) {
        if (!token.empty())
            tokens.push_back(token);
    }
    return tokens;
}

std::string StringifyPosition(float x, float y) {
    std::string msg = std::to_string(x) + "|" + std::to_string(y);
    return msg;
}

void SetupSDL() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "couldn't initialize SDL video module " << std::endl;

        exit(EXIT_FAILURE);
    }
    if (!SDL_CreateWindowAndRenderer("feneettttre", WINDOW_W, WINDOW_H, 0, &window, &renderer)) {
        std::cout << "couldn't create a window and a renderer" << std::endl;
    }
}

void WindowDraw() {
    SDL_SetRenderDrawColorFloat(renderer, 1.0f, 1.0f, 1.0f, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColorFloat(renderer, 1.0f, 0.0f, 0.0f, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderFillRect(renderer, &player);

    for (auto& [_, j] : joueurs) {
        SDL_RenderFillRect(renderer, &j.r);
    }

    SDL_RenderPresent(renderer);

}

Coordonnees ProcessKeypresses() {
    const bool* keystates = SDL_GetKeyboardState(0);
    Coordonnees dir{};

    if (keystates[SDL_SCANCODE_LEFT])  dir.x = -1;
    if (keystates[SDL_SCANCODE_RIGHT]) dir.x = 1;
    if (keystates[SDL_SCANCODE_UP])    dir.y = -1;
    if (keystates[SDL_SCANCODE_DOWN])  dir.y = 1;
    

    return dir;
}

bool Update() {

    SDL_Event event;
    Coordonnees dir{};

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_EVENT_QUIT:
            return false;
        }
    }

    dir = ProcessKeypresses();
    player.x += dir.x * MOVE_SPEED;
    player.y += dir.y * MOVE_SPEED;
    return true;
}

void CleanSDL() {
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
        joueurs[id] = { {0.0f, 0.0f, PLAYER_WH, PLAYER_WH} };
        break;
    case PacketTypes::PLAYER_DISCONNECT: 
        joueurs.erase(id);
        break;
    case PacketTypes::UPDATE_POSITION: {
        float x = std::stof(tokens[2]);
        float y = std::stof(tokens[3]);
        joueurs[id].r.x = x;
        joueurs[id].r.y = y;
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



    while (running) {
        start = (Uint32)SDL_GetTicks();

        running = Update();
        WindowDraw();
        MsgLoop(client);

        if (1000 / FPS > SDL_GetTicks() - start) {
            SDL_Delay(1000 / FPS - ((Uint32)SDL_GetTicks() - start));
        }

        if (packet_send_timer > packet_send_rate) {
            pos_string = StringifyPosition(player.x, player.y);
            SendPacket(peer, pos_string.c_str());
            packet_send_timer = 0;
        }

        packet_send_timer++;


        // Vérifier l'état de la connexion
        if (peer->state == ENET_PEER_STATE_DISCONNECTED) {
            std::cout << "Connexion perdue." << std::endl;
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

    player.w = PLAYER_WH;
    player.h = PLAYER_WH;
    player.x = 0;
    player.y = 0;

    runClient("127.0.0.1");
    enet_deinitialize();
    return EXIT_SUCCESS;
}