#pragma once
#include <enet/enet.h>
#include <stdexcept>

/// RAII wrapper around an ENetHost.
/// Now takes address by value so you can pass temporaries directly.
class ENetHostWrapper {
public:
    ENetHostWrapper(
        const ENetAddress*  address,
        size_t              maxClients,
        enet_uint8          channels,
        enet_uint32         incomingBandwidth = 0,
        enet_uint32         outgoingBandwidth = 0
    );
    ~ENetHostWrapper();

    ENetHost* get() const { return host_; }

    // non‐copyable
    ENetHostWrapper(const ENetHostWrapper&) = delete;
    ENetHostWrapper& operator=(const ENetHostWrapper&) = delete;

    // movable
    ENetHostWrapper(ENetHostWrapper&&) noexcept;
    ENetHostWrapper& operator=(ENetHostWrapper&&) noexcept;

private:
    ENetHost* host_;
    static bool   initialized_;
};
