// MainMenuScene.hpp
#pragma once
#include "Scene.hpp"
#include "GameScene.hpp"
#include "SDL3_ttf/SDL_ttf.h"


class MainMenuScene : public Scene {
public:
    MainMenuScene(SceneContext& ctx);
    ~MainMenuScene() override;
    void init() override;
    void processInput() override;
    void update() override;
    void render() override;
    bool wantsSceneChange() const override;
    std::unique_ptr<Scene> nextScene() override;

private:
    TTF_Font*    font_{ nullptr };
    SDL_Texture* textTexture_{ nullptr };
    bool         enterPressed_ = false;
    SDL_FRect    textDest_{};
    std::string skin_link_ = "";
    std::string  username_ = "";      // Input buffers
    std::string  ip_ = "127.0.0.1";   // Default IP
    std::string  port_ = "12345";
    int title_size = 0;
};