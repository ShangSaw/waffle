#include "server.hpp"
#include <iostream>
#include <string>


Server::Server(uint16_t port, size_t maxClients)
    : address_{ ENET_HOST_ANY, port }
    , host_(&address_,
        maxClients,
        1)
{
    std::cout << "Server listening on port "
        << address_.port
        << std::endl;
}

Server::~Server() = default;

void Server::run() {
    ENetEvent event;
    while (true) {
        while (enet_host_service(host_.get(), &event, 10) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                std::cout << "player connected" << std::endl;
                handleConnect(event);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                handleReceive(event);
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                handleDisconnect(event);
                break;
            default:
                break;
            }
        }
    }
}

void Server::handleConnect(const ENetEvent& event) {
    int newId = nextId_++;
    players_[newId] = Player{ newId, "",0.0f, 0.0f};
    peerToId_[event.peer] = newId;

    std::cout << "[CONNECT] New player with ID " << newId
        << " from peer " << event.peer
        << std::endl;

    // Send the new player's own ID
    {
        std::string idMsg = std::to_string(PacketTypes::SEND_OWN_ID)
            + "|" + std::to_string(newId);
        enet_peer_send(event.peer, 0,
            enet_packet_create(idMsg.c_str(), idMsg.size() + 1, ENET_PACKET_FLAG_RELIABLE));
    }

    // Notify all others
    std::string bc = std::to_string(PacketTypes::PLAYER_CONNECT)
        + "|" + std::to_string(newId) + "|0|0";

    for (auto& [peer, id] : peerToId_) {
        if (peer != event.peer) {
            enet_peer_send(peer, 0,
                enet_packet_create(bc.c_str(), bc.size() + 1, ENET_PACKET_FLAG_RELIABLE));
        }
    }

    // Send existing players to the newcomer, but exclude the new player's own ID
    std::cout << "---" << std::endl;
    for (auto& [id, p] : players_) {
        std::cout << "player with id: " << p.id << " was sent to newcomer" << std::endl;
        if (id != newId) {  // Skip the new player's own ID
            std::string msg = std::to_string(PacketTypes::PLAYER_CONNECT)
                + "|" + std::to_string(id)
                + "|" + std::to_string(p.x)
                + "|" + std::to_string(p.y);

            enet_peer_send(event.peer, 0,
                enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE));
            
            std::string skin_msg = std::to_string(PacketTypes::SKIN_LINK)
                + "|" + std::to_string(id)
                + "|" + p.skin_link;

            enet_peer_send(event.peer, 0,
                enet_packet_create(skin_msg.c_str(), skin_msg.size() + 1, ENET_PACKET_FLAG_RELIABLE));
        }
    }
}

void Server::handleReceive(const ENetEvent& event) {
    auto data = reinterpret_cast<char*>(event.packet->data);
    auto tokens = split_string(data);
    if (tokens.size() < 2) return;

    try {
        int type = std::stoi(tokens[0]);
        int playerId = std::stoi(tokens[1]);
        if (type == PacketTypes::UPDATE_POSITION && tokens.size() == 4) {
            float x = std::stof(tokens[2]), y = std::stof(tokens[3]);
            players_[playerId].x = x;
            players_[playerId].y = y;

            std::string msg = std::to_string(PacketTypes::UPDATE_POSITION)
                + "|" + std::to_string(playerId)
                + "|" + std::to_string(x)
                + "|" + std::to_string(y);
            for (auto& [peer, id] : peerToId_) {
                if (peer != event.peer) {
                    enet_peer_send(peer, 0,
                        enet_packet_create(msg.c_str(), msg.size() + 1,
                            ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT));
                }
            }
        }
        if (type == PacketTypes::SKIN_LINK && tokens.size() == 3) {
            // notify the others of a skin link
            players_[playerId].skin_link = tokens[2];

            std::string bc = std::to_string(PacketTypes::SKIN_LINK)
                + "|" + std::to_string(playerId) + "|" + tokens[2];

            for (auto& [peer, id] : peerToId_) {
                if (peer != event.peer) {
                    enet_peer_send(peer, 0,
                        enet_packet_create(bc.c_str(), bc.size() + 1, ENET_PACKET_FLAG_RELIABLE));
                }
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error parsing packet: " << e.what() << std::endl;
    }
}

void Server::handleDisconnect(const ENetEvent& event) {
    int id = peerToId_[event.peer];
    players_.erase(id);
    peerToId_.erase(event.peer);

    std::string msg = std::to_string(PacketTypes::PLAYER_DISCONNECT)
        + "|" + std::to_string(id);
    for (auto& [peer, _] : peerToId_) {
        enet_peer_send(peer, 0,
            enet_packet_create(msg.c_str(), msg.size() + 1, ENET_PACKET_FLAG_RELIABLE));
    }
}

int main() {
    try {
        // port, max players
        Server server{ 12345, 4};
        server.run();
    }
    catch (std::exception& e) {
        std::cerr << "Fatal: " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}