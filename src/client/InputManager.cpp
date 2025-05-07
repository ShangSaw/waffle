#include "InputManager.hpp"

void InputManager::pollEvents() {
    movement_ = { 0,0 };

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        switch (e.type) {
        case SDL_EVENT_QUIT:
            quit_ = true;
            break;
        case SDL_EVENT_KEY_UP:
            if (e.key.key == SDLK_RETURN) {
                toggleFS_ = !toggleFS_;
            }
        }
    }

    const bool* keys = SDL_GetKeyboardState(0);
    if (keys[SDL_SCANCODE_LEFT])  movement_.x = -1;
    if (keys[SDL_SCANCODE_RIGHT]) movement_.x = +1;
    if (keys[SDL_SCANCODE_UP])    movement_.y = -1;
    if (keys[SDL_SCANCODE_DOWN])  movement_.y = +1;
}
