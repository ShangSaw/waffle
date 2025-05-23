// GameScene.hpp
#pragma once
#include "Scene.hpp"
#include "MainMenuScene.hpp"

#include "NetworkClient.hpp"
#include "Player.hpp"
#include <unordered_map>
#include <filesystem>


class GameScene : public Scene {
public:

    GameScene(SceneContext& ctx);
    void init() override;
    void processInput() override;
    void update() override;
    void render() override;
    bool wantsSceneChange() const override;
    std::unique_ptr<Scene> nextScene() override;

private:
    std::unordered_map<int, Player> players_;
    Player                          me_;
    std::string						skin_path;
};
