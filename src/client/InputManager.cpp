#include "InputManager.hpp"
#include <iostream>

void InputManager::pollEvents() {
    movement_ = { 0,0 };

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        ImGui_ImplSDL3_ProcessEvent(&e);
        switch (e.type) {
        case SDL_EVENT_QUIT:
            quit_ = true;
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            size_.x = static_cast<float>(e.window.data1);
            size_.y = static_cast<float>(e.window.data2);
            break;
        }
    }

    const bool* keys = SDL_GetKeyboardState(0);
    if (keys[SDL_SCANCODE_LEFT])  movement_.x = -1;
    if (keys[SDL_SCANCODE_RIGHT]) movement_.x = +1;
    if (keys[SDL_SCANCODE_UP])    movement_.y = -1;
    if (keys[SDL_SCANCODE_DOWN])  movement_.y = +1;
}