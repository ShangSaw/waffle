#pragma once
#include "SDL3/SDL.h"

/// A simple client‐side Player/Entity struct.
struct Player {
    int      id = -1;
    SDL_FRect r = { 0, 0, 80.0f, 80.0f };
    SDL_FRect dr = { 0, 0, 80.0f, 80.0f };
};
