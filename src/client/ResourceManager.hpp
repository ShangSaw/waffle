#pragma once
#include <string>
#include <unordered_map>
#include "SDL3/SDL.h"
#include "SDL3_image/SDL_image.h"

#include "curl/curl.h"
#include <fstream>
#include <iostream>


/// Downloads PNGs via libcurl and caches SDL_Textures.
class ResourceManager {
public:
	ResourceManager(SDL_Renderer* ren);
	~ResourceManager();

	/// Returns nullptr if missing
	SDL_Texture*  get(const std::string& key);
	bool          downloadPNG(const std::string& url, const std::string& key);
	SDL_Texture*  loadTexture(const std::string& path);

private:
	SDL_Renderer* ren_;
	std::unordered_map<std::string, SDL_Texture*> cache_;
};