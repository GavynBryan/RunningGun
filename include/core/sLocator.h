#pragma once
#include "ResourceHandler.h"
#include "World.h"

class sLocator
{
private:
	static ResourceHandler<sf::Texture>* TextureHandler;

public:
	static void provide(ResourceHandler<sf::Texture>* _textures) {
		TextureHandler = _textures;
	}

	static ResourceHandler<sf::Texture>* getTextureHandler() { return TextureHandler; }

};