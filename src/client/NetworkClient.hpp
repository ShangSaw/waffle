#pragma once
#include <functional>
#include <string>
#include "ENetHostWrapper.hpp"
#include "packets.hpp"
#include "util.hpp"

class NetworkClient {
public:
	NetworkClient(const std::string& host, uint16_t port);
	~NetworkClient();

	/// Call every frame (or on a timer) to service ENet.
	void poll();

	void handleReceive(const ENetEvent& event);

	/// Sends: UPDATE_POSITION|id|x|y
	void sendPosition(int id, float x, float y);
	void sendPacket(std::string& data);

	// Callbacks you bind in Game:
	std::function<void(int /*id*/, float, float)>			onPlayerConnect;
	std::function<void(int /*id*/, std::string)>			onSkinLink;
	std::function<void(int /*id*/)>			                onPlayerDisconnect;
	std::function<void(int /*id*/, float, float)>           onPlayerUpdate;
	std::function<void(int /*yourId*/)>                     onOwnId;

private:
	bool connected = false;
	ENetHostWrapper host_;
	ENetPeer* peer_;
};
