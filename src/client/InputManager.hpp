#pragma once
#include "SDL3/SDL.h"
#include "util.hpp"

/// Wraps SDL event polling and exposes movement + quit/fullscreen.
class InputManager {
public:
	void			pollEvents();
	Coordonnees     movement()   const { return movement_; }
	bool			quitting()   const { return quit_; }
	bool			toggleFS() { return std::exchange(toggleFS_, false); }

private:
	bool			quit_ = false;
	bool			toggleFS_ = false;
	Coordonnees		movement_;
};