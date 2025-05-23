#include "imgui.h"
#include "imgui_stdlib.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "MainMenuScene.hpp"
#include <iostream>


MainMenuScene::MainMenuScene(SceneContext& ctx)
    : Scene(ctx)
{

    // Initialize SDL_ttf
    if (!TTF_Init()) {
        std::cout << "Error initializing SDL_ttf" << std::endl;
    }

    font_ = TTF_OpenFont("alagard.ttf", 24);
    if (!font_) {
        std::cout << "Failed to load font" << std::endl;
    }

    textDest_.x = 0;
    textDest_.y = 0;
}


MainMenuScene::~MainMenuScene() {
    if (textTexture_) {
        SDL_DestroyTexture(textTexture_);
    }
    if (font_) {
        TTF_CloseFont(font_);
    }
}

void MainMenuScene::init() {
    font_ = TTF_OpenFont("alagard.ttf", 45);
    if (!font_) {
        std::cerr << "TTF_OpenFont: " << std::endl;
    }

    std::string gameName = "Wacky Assholes Fighting For Loot: EX";
    SDL_Color color{ 0,0,0,255 };
    SDL_Surface* surf = TTF_RenderText_Solid(font_, gameName.c_str(), gameName.size(), color);
    if (!surf) {
        std::cerr << "TTF_RenderText_Solid: " << std::endl;
        return;
    }
    textTexture_ = SDL_CreateTextureFromSurface(ctx_.renderer.renderer_, surf);
    textDest_.x = static_cast<float>(1280/2 - surf->w/2);
    textDest_.y = 50;
    textDest_.w = static_cast<float>(surf->w);
    textDest_.h = static_cast<float>(surf->h);
    SDL_DestroySurface(surf);
}

void MainMenuScene::processInput() {

    ctx_.input.pollEvents();
    if (ctx_.input.quitting()) quit_ = true;
}

void MainMenuScene::update() {
}

void MainMenuScene::render() {
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    ctx_.renderer.clear();
    if (textTexture_) {
        SDL_RenderTexture(ctx_.renderer.renderer_, textTexture_, nullptr, &textDest_);
    }

    // --- ImGUI Window ---
    // Center the window and set size
    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_Appearing);

    ImGui::Begin("Connection Settings", nullptr,
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize
    );

    // Larger font for window
    ImGui::SetWindowFontScale(1.2f);

    // Username input
    ImGui::Text("Username:");
    ImGui::InputText("##username", &username_);

    // IP Address input
    ImGui::Text("IP Address:");
    ImGui::InputText("##ip", &ip_);

    // Port input
    ImGui::Text("Port:");
    ImGui::InputText("##port", &port_);

    // Connect/Quit buttons
    if (ImGui::Button("Connect", ImVec2(-FLT_MIN, 40))) {
        ctx_.network.reconnect(ip_, std::stoi(port_));
        enterPressed_ = true;
    }

    if (ImGui::Button("Quit", ImVec2(-FLT_MIN, 40))) {
        std::cout << "on quitte le menu" << std::endl;
        quit_ = true;
    }

    if (ImGui::CollapsingHeader("Options")) {
        // Fullscreen toggle
        bool currentFullscreen = (SDL_GetWindowFlags(ctx_.renderer.window_) & SDL_WINDOW_FULLSCREEN) != 0;
        if (ImGui::Checkbox("Fullscreen", &currentFullscreen)) {
            SDL_SetWindowFullscreen(ctx_.renderer.window_, currentFullscreen);
        }
    }

    ImGui::End();

    // Finalize and present
    ImGui::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), ctx_.renderer.renderer_);
    ctx_.renderer.present();
}

bool MainMenuScene::wantsSceneChange() const {
    return enterPressed_;
}

std::unique_ptr<Scene> MainMenuScene::nextScene() {
    if (enterPressed_) {
        // renvoyer la nouvelle scène de jeu
        return std::make_unique<GameScene>(ctx_);
    }
    return nullptr;
}