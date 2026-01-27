#include <core/ResourceHandler.h>
#include <stdexcept>

ResourceHandler::ResourceHandler(SDL_Renderer* renderer)
	: mRenderer(renderer)
{
}

ResourceHandler::~ResourceHandler()
{
	flush();
}

void ResourceHandler::load(const std::string& _filename)
{
	// Check if already loaded
	if (mResources.find(_filename) != mResources.end()) {
		return;
	}

	SDL_Texture* texture = IMG_LoadTexture(mRenderer, _filename.c_str());
	if (!texture) {
		throw std::runtime_error("Could not load " + _filename + ": " + SDL_GetError());
	}

	mResources[_filename] = texture;
}

SDL_Texture* ResourceHandler::get(const std::string& _filename)
{
	auto found = mResources.find(_filename);
	assert(found != mResources.end());
	return found->second;
}

void ResourceHandler::flush()
{
	for (auto& pair : mResources) {
		if (pair.second) {
			SDL_DestroyTexture(pair.second);
		}
	}
	mResources.clear();
}
