#include <core/ResourceHandler.h>
#include <stdexcept>

ResourceHandler::ResourceHandler(SDL_Renderer* renderer)
	: Renderer(renderer)
{
}

ResourceHandler::~ResourceHandler()
{
	Flush();
}

void ResourceHandler::Load(const std::string& _filename)
{
	// Check if already loaded
	if (Resources.find(_filename) != Resources.end()) {
		return;
	}

	SDL_Texture* _texture = IMG_LoadTexture(Renderer, _filename.c_str());
	if (!_texture) {
		throw std::runtime_error("Could not load " + _filename + ": " + SDL_GetError());
	}

	Resources[_filename] = _texture;
}

SDL_Texture* ResourceHandler::Get(const std::string& _filename)
{
	auto _found = Resources.find(_filename);
	assert(_found != Resources.end());
	return _found->second;
}

void ResourceHandler::Flush()
{
	for (auto& _pair : Resources) {
		if (_pair.second) {
			SDL_DestroyTexture(_pair.second);
		}
	}
	Resources.clear();
}
