#pragma once
#include <unordered_map>
#include <string>
#include <algorithm>
#include <filesystem>
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "NetworkClient.hpp"
#include "Player.hpp"

class Game {
public:
	Game(const std::string& server, uint16_t port);
	void run();
	
	SDL_FRect camera_{};

private:
	void       processInput();
	void       update();
	void       render();

	InputManager					input_;
	Renderer						renderer_;
	ResourceManager					resources_;
	NetworkClient					network_;
	std::unordered_map<int, Player> players_;
	Player							me_{};
	std::string						skin_path;
	bool							running_ = true;
};
