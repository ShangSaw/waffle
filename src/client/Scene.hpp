#pragma once
#include "SceneContext.hpp"
#include <memory>

// Interface de base pour toutes les scènes
class Scene {
public:
    virtual ~Scene() = default;

    Scene(SceneContext& ctx) : ctx_(ctx) {}

    // appelé une seule fois à la création
    virtual void init() = 0;

    // gère les événements SDL
    virtual void processInput() = 0;

    // met à jour la logique (delta time si besoin)
    virtual void update() = 0;

    // dessine la scène
    virtual void render() = 0;

    // indique si on veut changer de scène
    virtual bool wantsSceneChange() const = 0;
    virtual std::unique_ptr<Scene> nextScene() = 0;

    bool quit_ = false;

protected:
    // accès direct en protected
    SceneContext& ctx_;
};
