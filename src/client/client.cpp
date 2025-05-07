#include "Game.hpp"

int main(int argc, char** argv) {
    try {
        Game game{ "127.0.0.1", 12345 };
        game.run();
        return EXIT_SUCCESS;
    }
    catch (std::exception& e) {
        std::cerr << "Caught exception in main(): " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    catch (...) {
        std::cerr << "Caught unknown exception in main()\n";
        return EXIT_FAILURE;
    }
}


/*

#include <iostream>
#include <fstream>

#include "curl/curl.h"
#include "enet/enet.h"
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"

#include "client.hpp"
#include "util.hpp"

static Joueur player;
static SDL_FRect camera;


std::string StringifyPosition(float x, float y) {
    return std::to_string(PacketTypes::UPDATE_POSITION) +
        "|" + std::to_string(player.id) +
        "|" + std::to_string(x) +
        "|" + std::to_string(y);
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
    float newX = player.r.x + dir.x * MOVE_SPEED;
    float newY = player.r.y + dir.y * MOVE_SPEED;

    // Clamp the player's position to the world boundaries
    newX = max(newX, (float)WORLD_MIN_X);
    newX = min(newX, (float)(WORLD_MAX_X - PLAYER_WH));
    newY = max(newY, (float)WORLD_MIN_Y);
    newY = min(newY, (float)(WORLD_MAX_Y - PLAYER_WH));

    player.r.x = newX;
    player.r.y = newY;

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

void SendPacket(ENetPeer* peer, const char* data) {
    ENetPacket* packet = enet_packet_create(data, strlen(data) + 1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
    enet_peer_send(peer, 0, packet);
}

void ParsePacket(ENetEvent* e) {


    std::string data(reinterpret_cast<char*>(e->packet->data));
    std::vector<std::string> tokens = split_string(data);
    int packet_type = std::stoi(tokens[0]);

    switch (packet_type) {
    case PacketTypes::PLAYER_CONNECT: {
        if (tokens.size() < 4) break;
        int id = std::stoi(tokens[1]);
        float x = std::stof(tokens[2]);
        float y = std::stof(tokens[3]);
        joueurs[id] = { id, {x, y, PLAYER_WH, PLAYER_WH}, {0,0,0,0}, nullptr };
        break;
    }

    case PacketTypes::UPDATE_POSITION: {
        if (tokens.size() < 4) break;
        int id = std::stoi(tokens[1]);
        float x = std::stof(tokens[2]);
        float y = std::stof(tokens[3]);
        joueurs[id].r.x = x;
        joueurs[id].r.y = y;
        break;
    }
    case PacketTypes::SEND_OWN_ID:
        int id = std::stoi(tokens[1]);
        player.id = id;
        std::cout << "Mon ID client est : " << player.id << std::endl;
        break;
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
            std::cout << "[CLIENT] Envoie position : " << pos_string << std::endl;
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


    // code pour telecharger un skin custom pour le joueur
    std::string imageUrl = "https://pngimg.com/uploads/anime_girl/anime_girl_PNG93.png";
    std::string destination = "image_renamed.png";

    if (downloadPngImage(imageUrl, destination)) {
        std::cout << "Téléchargement réussi !" << std::endl;

        SDL_Surface* surface = IMG_Load("image_renamed.png");
        if (!surface) {
            SDL_Log("Erreur lors du chargement de l'image : %s", SDL_GetError());
            return EXIT_FAILURE;
        }

        player.tex = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_DestroySurface(surface);

        if (!player.tex) {
            SDL_Log("Erreur lors de la création de la texture : %s", SDL_GetError());
            return EXIT_FAILURE;
        }
    }
    else {
        std::cout << "Échec du téléchargement." << std::endl;
    }

    // fin du code pour le skin custom

    player.r.w = PLAYER_WH;
    player.r.h = PLAYER_WH;
    player.r.x = 0;
    player.r.y = 0;

    player.id = 0;

    camera.w = WINDOW_W;
    camera.h = WINDOW_H;
    camera.x = 0;
    camera.y = 0;

    runClient("127.0.0.1");
    enet_deinitialize();
    
    return EXIT_SUCCESS;
}

*/