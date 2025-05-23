#pragma once
#include <string>
#include <iostream>
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"
#include "util.hpp"
#include "Player.hpp"

/// Owns SDL_Window + SDL_Renderer; handles camera movement and drawing.
class Renderer {
public:
    Renderer(int width, int height);
    ~Renderer();

    /// Clear the screen to white
    void clear();

    /// Present the rendered frame
    void present();

    /// Center the camera on a world rectangle
    void setCamera(const SDL_FRect& worldRect);

    /// Draw a world rectangle with optional texture
    void draw(const SDL_FRect& worldRect, SDL_Texture* texture = nullptr);


    SDL_Window*   window_   = nullptr;
    SDL_Renderer* renderer_ = nullptr;
    SDL_Texture* dtarget_   = nullptr;

    SDL_FRect     camera_   = { 0, 0, 0, 0 };
};
