#pragma once
#include "SDL3/SDL.h"
#include "util.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_sdlrenderer3.h"

/// Wraps SDL event polling and exposes movement + quit/fullscreen.
class InputManager {
public:
	void			pollEvents();
	Coordonnees     movement()   const { return movement_; }
	bool			quitting()   const { return quit_; }
	Coordonnees		size_{ 1280, 720 };

private:
	bool			quit_ = false;
	Coordonnees		movement_;
};