#include "Renderer.hpp"

Renderer::Renderer(int width, int height) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "couldn't initialize SDL video module " << std::endl;

        exit(EXIT_FAILURE);
    }
    if (!SDL_CreateWindowAndRenderer("feneettttre", width, height, 0, &window_, &renderer_)) {
        std::cerr << "couldn't create a window and a renderer" << std::endl;
    }


    // Initialize camera size to the window size
    int w, h;
    SDL_GetWindowSize(window_, &w, &h);
    camera_.w = static_cast<float>(w);
    camera_.h = static_cast<float>(h);
}

Renderer::~Renderer() {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
}

void Renderer::clear() {
    SDL_SetRenderDrawColor(renderer_, 255, 255, 255, 255);
    SDL_RenderClear(renderer_);
}

void Renderer::present() {
    SDL_RenderPresent(renderer_);
}

void Renderer::setCamera(const SDL_FRect& worldRect) {
    // Center camera on worldRect
    camera_.x = worldRect.x + worldRect.w / 2.0f - camera_.w / 2.0f;
    camera_.y = worldRect.y + worldRect.h / 2.0f - camera_.h / 2.0f;
}

void Renderer::draw(const SDL_FRect& worldRect, SDL_Texture* texture) {
    // Compute destination rect in screen space
    SDL_FRect dst{};
    dst.x = worldRect.x - camera_.x;
    dst.y = worldRect.y - camera_.y;
    dst.w = worldRect.w;
    dst.h = worldRect.h;

    if (texture) {
        // Simply copy the full texture to dst
        SDL_RenderTexture(renderer_, texture, nullptr, &dst);
    }
    else {
        // Draw a red rectangle
        SDL_SetRenderDrawColor(renderer_, 255, 0, 0, 255);
        SDL_RenderFillRect(renderer_, &dst);
    }
}