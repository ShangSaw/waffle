#pragma once
#include <enet/enet.h>
#include <unordered_map>
#include "ENetHostWrapper.hpp"
#include "packets.hpp"
#include "util.hpp"

struct Player {
    int   id;
    std::string skin_link = "";
    float x, y;
};

/// Encapsulates the main server loop, player registry, and broadcast logic.
class Server {
public:
    Server(uint16_t port, size_t maxClients = 4);
    ~Server();

    /// Blocks and runs forever (or until an exception is thrown).
    void run();

private:
    void handleConnect(const ENetEvent& event);
    void handleReceive(const ENetEvent& event);
    void handleDisconnect(const ENetEvent& event);

    ENetAddress                           address_;   // ← store the address here
    ENetHostWrapper host_;
    int nextId_ = 0;
    std::unordered_map<int, Player>           players_;
    std::unordered_map<ENetPeer*, int>        peerToId_;
};