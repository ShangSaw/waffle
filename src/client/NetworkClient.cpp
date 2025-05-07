#include "NetworkClient.hpp"
#include <iostream>

NetworkClient::NetworkClient(const std::string& host, uint16_t port)
    : host_(nullptr, 1, 1)
    , peer_(nullptr)
{
    std::cout << "Initializing ENet client..." << std::endl; // <-- Add

    ENetAddress serverAddr;
    enet_address_set_host(&serverAddr, host.c_str());
    serverAddr.port = port;

    std::cout << "Attempting to connect to " << host << ":" << port << std::endl; // <-- Add

    peer_ = enet_host_connect(host_.get(), &serverAddr, 1, 0);
    if (!peer_) throw std::runtime_error("ENet connect failed");
    std::cout << "connected to server" << std::endl;
}

NetworkClient::~NetworkClient() {
    if (peer_) enet_peer_disconnect(peer_, 0);
}

void NetworkClient::sendPacket(std::string& data) {
    ENetPacket* packet = enet_packet_create(data.c_str(), data.size()  + 1, ENET_PACKET_FLAG_UNRELIABLE_FRAGMENT);
    enet_peer_send(peer_, 0, packet);
}


void NetworkClient::poll() {
    ENetEvent ev;
    while (enet_host_service(host_.get(), &ev, 0) > 0) {
        switch (ev.type) {
        case ENET_EVENT_TYPE_CONNECT: // Add this case
            connected = true;
            std::cout << "Connection to server confirmed." << std::endl;
            break;
        case ENET_EVENT_TYPE_RECEIVE:
            handleReceive(ev);
            break;
        case ENET_EVENT_TYPE_DISCONNECT:
            connected = false;
            std::cout << "Disconnected from server." << std::endl;
            break;
        default:
            break;
        }
        enet_packet_destroy(ev.packet);
    }
}

void NetworkClient::handleReceive(const ENetEvent& event) {
    try {
        auto data = reinterpret_cast<char*>(event.packet->data);
        auto tok = split_string(data);
        if (tok.empty()) {
            enet_packet_destroy(event.packet);
        }

        int type = std::stoi(tok[0]);
        switch (type) {
        case PacketTypes::PLAYER_CONNECT:
            if (tok.size() >= 4) {
                onPlayerConnect(std::stoi(tok[1]), std::stof(tok[2]), std::stof(tok[3]));

            }
            break;
        case PacketTypes::UPDATE_POSITION:
            if (tok.size() >= 4) {
                onPlayerUpdate(std::stoi(tok[1]), std::stof(tok[2]), std::stof(tok[3]));
            }
            break;
        case PacketTypes::PLAYER_DISCONNECT:
            if (tok.size() >= 2) {
                onPlayerDisconnect(std::stoi(tok[1]));
            }
            break;
        case PacketTypes::SEND_OWN_ID:
            if (tok.size() >= 2) {
                onOwnId(std::stoi(tok[1]));
            }
            break;
        case PacketTypes::SKIN_LINK:
            if (tok.size() >= 3) {
                onSkinLink(std::stoi(tok[1]), tok[2]);
            }
            break;
        default:
            break;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Packet error: " << e.what() << std::endl;
    }
}

void NetworkClient::sendPosition(int id, float x, float y) {
    std::string msg = std::to_string(PacketTypes::UPDATE_POSITION)
        + "|" + std::to_string(id)
        + "|" + std::to_string(x)
        + "|" + std::to_string(y);

    sendPacket(msg);
}
