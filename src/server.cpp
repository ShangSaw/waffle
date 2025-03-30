#include <enet/enet.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdlib> 
#include "packets.h"


// structure temporaire 
struct Coordonnees {
    float x, y;
};

struct Joueur {
    int id;
    Coordonnees coord;
};
// structure temporaire fin

int main(int argc, char** argv) {
    // initialisation d'Enet
    if (enet_initialize() != 0) {
        std::cerr << "Erreur : impossible d'initialiser ENet." << std::endl;
        return EXIT_FAILURE;
    }

    // création du serveur
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    ENetHost* server = enet_host_create(&address, 4, 1, 0, 0);
    if (!server) {
        std::cerr << "Erreur : impossible de créer le serveur." << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Serveur ENet lancé sur le port  : "<< address.port << std::endl;

    //a changer, 
    std::unordered_map<ENetPeer*, Joueur> joueurs;
    int prochain_id = 0;
    // a changer fin


    // réceptions des évènements
    ENetEvent event;
    while (true) {
        while (enet_host_service(server, &event, 10) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_CONNECT:{
                    std::cout << "Joueur connecté port:" << event.peer->address.port << std::endl;

                    std::string msg_recup;
                    // on envoie au joueur tous les joueurs présents sur la map
                    for (auto& [_, j] : joueurs) {
                        msg_recup = std::to_string(PacketTypes::PLAYER_CONNECT) + "|" + std::to_string(j.id);
                        ENetPacket* paquet = enet_packet_create(msg_recup.c_str(), msg_recup.length() + 1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
                        enet_peer_send(event.peer, 0, paquet);
                    }

                    Joueur joueur;
                    joueur.id = prochain_id++;
                    joueur.coord.x = 0.0f;
                    joueur.coord.y = 0.0f;
                    joueurs[event.peer] = joueur;
                    // pour les lecteurs endurcies
                    // joueurs[event.peer] = {prochain_id++, {0.0f, 0.0f}};
                    std::string msg_envoie = std::to_string(PacketTypes::PLAYER_CONNECT) + "|" + std::to_string(joueur.id);
                    
                    // broadcast
                    for (auto& [peer, j] : joueurs) {
                        if (peer != event.peer) {
                            // pour l'instant c'est du tcp (reliable) pour le test mais plus tard ça sera unreliable
                            ENetPacket* paquet = enet_packet_create(msg_envoie.c_str(), msg_envoie.length() + 1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
                            enet_peer_send(peer, 0, paquet);
                        }
                    }

                    break;
                }

                case ENET_EVENT_TYPE_RECEIVE:{               
                    // conversion en string je prevoie de recevoir x|y pour l'instant
                    std::string msg(reinterpret_cast<char*>(event.packet->data));
                    enet_packet_destroy(event.packet);

                    // on fera propre plus tard, là j'ai trouvé 
                    // des fonctions randoms sur les strings   
                    size_t sep = msg.find('|');
                    float x = std::stof(msg.substr(0, sep));
                    float y = std::stof(msg.substr(sep + 1));

                    //std::cout << "position x:" << x << " position y:" << y << std::endl;
                    
                    joueurs[event.peer].coord.x = x;
                    joueurs[event.peer].coord.y = y;
                    int id = joueurs[event.peer].id;
                    
                    //CORRIGÉ PAR KENNETH: mtn on envoie 2|id|x|y pour pas confondre avec les autres types de paquet
                    // envoie aux autres clients : j'envoie id|x|y pour l'instant
                    std::string msg_envoie = std::to_string(PacketTypes::UPDATE_POSITION) + "|" + std::to_string(id) + "|" + std::to_string(x) + "|" + std::to_string(y);
                    

                    // broadcast
                    for (auto& [peer, j] : joueurs) {
                        if (peer != event.peer) {
                            // pour l'instant c'est du tcp (reliable) pour le test mais plus tard ça sera unreliable
                            ENetPacket* paquet = enet_packet_create(msg_envoie.c_str(), msg_envoie.length() + 1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
                            enet_peer_send(peer, 0, paquet);
                        }
                    }
                    
                    break;
                }
                case ENET_EVENT_TYPE_DISCONNECT:{
                    std::cout << "Un client s'est déconnecté." << std::endl;

                    std::string msg_envoie = std::to_string(PacketTypes::PLAYER_DISCONNECT) + "|" + std::to_string(joueurs[event.peer].id);

                    // broadcast
                    for (auto& [peer, j] : joueurs) {
                        if (peer != event.peer) {
                            // pour l'instant c'est du tcp (reliable) pour le test mais plus tard ça sera unreliable
                            ENetPacket* paquet = enet_packet_create(msg_envoie.c_str(), msg_envoie.length() + 1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
                            enet_peer_send(peer, 0, paquet);
                        }
                    }
                    joueurs.erase(event.peer);
                    break;
                }   
                default:
                    break;
            }
        }
    }

    enet_host_destroy(server);
    atexit(enet_deinitialize);

    return 0;
}