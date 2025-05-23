#pragma once
#include <unordered_map>
#include <string>
#include <algorithm>
#include "InputManager.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "NetworkClient.hpp"
#include "Player.hpp"


#include "Scene.hpp"
#include "MainMenuScene.hpp"
#include "GameScene.hpp"

class Game {
public:
    Game(const std::string& server, uint16_t port);
	void run();
private:

    InputManager    input_;
    Renderer        renderer_;
    ResourceManager resources_;
    NetworkClient   network_;
    SceneContext    context_;
	std::unique_ptr<Scene>          currentScene_;
	bool							running_ = true;
};
