#include "server.hpp"
#include <iostream>
#include <string>
#include "nlohmann/json.hpp"
using json = nlohmann::json;


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
    players_[newId] = Player{ newId, "","",0.0f, 0.0f};
    peerToId_[event.peer] = newId;

    std::cout << "---" << std::endl;
    std::cout << "[CONNECT] New player with ID " << newId
        << " from peer " << event.peer
        << std::endl;

    // Send the new player's own ID
    {
        json msgJson = {
            {"type", PacketTypes::SEND_OWN_ID},
            {"id", newId},
        };

        enet_peer_send(event.peer, 0,
            enet_packet_create(msgJson.dump().c_str(), msgJson.dump().size() + 1, ENET_PACKET_FLAG_RELIABLE));
    }

    // Notify all others

    json bcJson = {
        {"type", PacketTypes::PLAYER_CONNECT},
        {"id", newId},
        {"x", 0},
        {"y", 0},
    };

    for (auto& [peer, id] : peerToId_) {
        if (peer != event.peer) {
            enet_peer_send(peer, 0,
                enet_packet_create(bcJson.dump().c_str(), bcJson.dump().size() + 1, ENET_PACKET_FLAG_RELIABLE));
        }
    }

    // Send existing players to the newcomer, but exclude the new player's own ID
    for (auto& [id, p] : players_) {
        std::cout << "player with id: " << p.id << " was sent to newcomer" << std::endl;
        if (id != newId) {  // Skip the new player's own ID

            json pJson = {
                {"type", PacketTypes::PLAYER_CONNECT},
                {"id", id},
                {"x", p.x},
                {"y", p.y},
            };

            enet_peer_send(event.peer, 0,
                enet_packet_create(pJson.dump().c_str(), pJson.dump().size() + 1, ENET_PACKET_FLAG_RELIABLE));

            json skinJson = {
                {"type", PacketTypes::SKIN_LINK},
                {"id", id},
                {"link", p.skin_link},
            };

            enet_peer_send(event.peer, 0,
                enet_packet_create(skinJson.dump().c_str(), skinJson.dump().size() + 1, ENET_PACKET_FLAG_RELIABLE));

            json usernameJson{
                {"type", PacketTypes::SET_USERNAME},
                {"id", id},
                {"username", p.username},
            };

            enet_peer_send(event.peer, 0,
                enet_packet_create(usernameJson.dump().c_str(), usernameJson.dump().size() + 1, ENET_PACKET_FLAG_RELIABLE));
        }
    }
}

void Server::handleReceive(const ENetEvent& event) {
    auto data = reinterpret_cast<char*>(event.packet->data);
    json packet = json::parse(data);

    try {
        int type = packet["type"];
        int playerId = packet["id"];

        if (type == PacketTypes::UPDATE_POSITION) {
            std::cout << "position received: " << packet.dump() << std::endl;
            players_[playerId].x = packet["x"];
            players_[playerId].y = packet["y"];

            json pJson = {
                {"type", PacketTypes::UPDATE_POSITION},
                {"id", playerId },
                {"x", packet["x"]},
                {"y", packet["y"]},
            };

            for (auto& [peer, id] : peerToId_) {
                if (peer != event.peer) {
                    enet_peer_send(peer, 0,
                        enet_packet_create(pJson.dump().c_str(), pJson.dump().size() + 1,
                            ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT));
                }
            }
        }
        if (type == PacketTypes::SKIN_LINK) {
            // notify the others of a skin link
            players_[playerId].skin_link = packet["link"];

            json bcJson = {
                {"type", PacketTypes::SKIN_LINK},
                {"id", playerId},
                {"link", packet["link"]},
            };

            for (auto& [peer, id] : peerToId_) {
                if (peer != event.peer) {
                    enet_peer_send(peer, 0,
                        enet_packet_create(bcJson.dump().c_str(), bcJson.dump().size() + 1, ENET_PACKET_FLAG_RELIABLE));
                }
            }
        }

        if (type == PacketTypes::SET_USERNAME) {
            players_[playerId].username = packet["username"];
            std::cout << "[SET_USERNAME] Player " << playerId
                << " set username to: " << players_[playerId].username << std::endl;

            json bcJson = {
                {"type", PacketTypes::SET_USERNAME},
                {"id", playerId},
                {"username", packet["username"]},
            };

            for (auto& [peer, id] : peerToId_) {
                if (peer != event.peer) {
                    enet_peer_send(peer, 0,
                        enet_packet_create(bcJson.dump().c_str(), bcJson.dump().size() + 1, ENET_PACKET_FLAG_RELIABLE));
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

    json msgJson = {
        {"type", PacketTypes::PLAYER_DISCONNECT},
        {"id", id}
    };

    for (auto& [peer, _] : peerToId_) {
        enet_peer_send(peer, 0,
            enet_packet_create(msgJson.dump().c_str(), msgJson.dump().size() + 1, ENET_PACKET_FLAG_RELIABLE));
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