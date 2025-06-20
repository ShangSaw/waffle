#include "NetworkClient.hpp"
#include <iostream>
#include "nlohmann/json.hpp"
using json = nlohmann::json;

NetworkClient::NetworkClient(const std::string& host, uint16_t port)
    : host_(nullptr, 1, 1)
    , peer_(nullptr)
{
    std::cout << "Initializing ENet client..." << std::endl;

    ENetAddress serverAddr;
    enet_address_set_host(&serverAddr, host.c_str());
    serverAddr.port = port;

    std::cout << "Attempting to connect to " << host << ":" << port << std::endl;

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

void NetworkClient::reconnect(std::string host, int port)
{
    enet_peer_disconnect_now(peer_, 0);

    ENetAddress serverAddr;
    enet_address_set_host(&serverAddr, host.c_str());
    serverAddr.port = port;

    std::cout << "Attempting to reconnect to " << host << ":" << port << std::endl; // <-- Add
    peer_ = enet_host_connect(host_.get(), &serverAddr, 1, 0);
    if (!peer_) throw std::runtime_error("ENet connect failed");
    std::cout << "connected to server" << std::endl;
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
        json packet = json::parse(data);

        int type = packet["type"];
        switch (type) {
        case PacketTypes::PLAYER_CONNECT:
            onPlayerConnect(packet["id"], packet["x"], packet["y"]);
            break;

        case PacketTypes::UPDATE_POSITION:
            onPlayerUpdate(packet["id"], packet["x"], packet["y"]);
            break;
        case PacketTypes::PLAYER_DISCONNECT:
            onPlayerDisconnect(packet["id"]);
            break;
        case PacketTypes::SEND_OWN_ID:
            onOwnId(packet["id"]);
            break;
        case PacketTypes::SKIN_LINK:
            onSkinLink(packet["id"], packet["link"]);
            break;
        case PacketTypes::SET_USERNAME:
            onSetUsername(packet["id"], packet["username"]);
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
    json msgJson = {
        {"type", PacketTypes::UPDATE_POSITION},
        {"id", id},
        {"x", x},
        {"y", y}
    };
    sendPacket(msgJson.dump());
}
