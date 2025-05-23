#include "Game.hpp"


Game::Game(const std::string& server, uint16_t port)
    : renderer_(1280, 720)
    , resources_(renderer_.renderer_)
    , network_(server, port)
    , context_{ input_, renderer_, resources_, network_ }
{
}

void Game::run() {

    currentScene_ = std::make_unique<MainMenuScene>(context_);
    currentScene_->init();

    const int FPS = 60;
    const Uint64 frameDelay = 1000 / FPS;

    Uint32 frameStart;
    Uint32 frameTime;

    while (!currentScene_.get()->quit_) {

        frameStart = static_cast<Uint32>(SDL_GetTicks());

        currentScene_->processInput();
        currentScene_->update();

    
        currentScene_->render();

        if (currentScene_->wantsSceneChange()) {
            std::cout << "let me in!" << std::endl;
            auto next = currentScene_->nextScene();
            if (!next) break;           // on quitte l’application
            currentScene_ = std::move(next);
            currentScene_->init();
        }

        frameTime = static_cast<Uint32>(SDL_GetTicks() - frameStart);

        if (frameDelay > frameTime) {
            SDL_Delay(static_cast<Uint32>(frameDelay - frameTime));
        }
    }
}