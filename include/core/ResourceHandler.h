#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <map>
#include <string>
#include <memory>
#include <stdexcept>
#include <cassert>

class ResourceHandler
{
public:
	ResourceHandler(SDL_Renderer* renderer);
	~ResourceHandler();

	void					Load(const std::string& _filename);
	SDL_Texture*			Get(const std::string& _filename);
	void					Flush();

private:
	SDL_Renderer* Renderer;
	std::map<std::string, SDL_Texture*> Resources;
};
