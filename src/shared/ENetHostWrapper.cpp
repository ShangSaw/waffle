#include "ENetHostWrapper.hpp"

bool ENetHostWrapper::initialized_ = false;

ENetHostWrapper::ENetHostWrapper(
    const ENetAddress* address,
    size_t              maxClients,
    enet_uint8          channels,
    enet_uint32         incomingBandwidth,
    enet_uint32         outgoingBandwidth
) : host_(nullptr)
{
    if (!initialized_) {
        if (enet_initialize() != 0)
            throw std::runtime_error("ENet initialization failed");
        initialized_ = true;
    }

    host_ = enet_host_create(
        address,
        static_cast<enet_uint32>(maxClients),
        channels,
        incomingBandwidth,
        outgoingBandwidth
    );
    if (!host_) {
        throw std::runtime_error("Failed to create ENet host");
    }
}

ENetHostWrapper::~ENetHostWrapper() {
    if (host_) {
        enet_host_destroy(host_);
        host_ = nullptr;
    }
    if (initialized_) {
        enet_deinitialize();
        initialized_ = false;
    }
}

ENetHostWrapper::ENetHostWrapper(ENetHostWrapper&& other) noexcept
    : host_(other.host_)
{
    other.host_ = nullptr;
}

ENetHostWrapper& ENetHostWrapper::operator=(ENetHostWrapper&& other) noexcept {
    if (this != &other) {
        if (host_) enet_host_destroy(host_);
        host_ = other.host_;
        other.host_ = nullptr;
    }
    return *this;
}
