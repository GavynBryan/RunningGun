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

	void					load(const std::string& _filename);
	SDL_Texture*			get(const std::string& _filename);
	void					flush();

private:
	SDL_Renderer* mRenderer;
	std::map<std::string, SDL_Texture*> mResources;
};
