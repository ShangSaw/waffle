#pragma once

#include "InputManager.hpp"
#include "Renderer.hpp"
#include "ResourceManager.hpp"
#include "NetworkClient.hpp"

// Regroupe tout ce dont vos scènes ont besoin:
struct SceneContext {
    InputManager& input;
    Renderer& renderer;
    ResourceManager& resources;
    NetworkClient& network;
};
