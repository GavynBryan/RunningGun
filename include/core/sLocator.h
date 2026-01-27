#pragma once
#include "ResourceHandler.h"
#include "World.h"

class sLocator
{
private:
	static ResourceHandler* TextureHandler;

public:
	static void provide(ResourceHandler* _textures) {
		TextureHandler = _textures;
	}

	static ResourceHandler* getTextureHandler() { return TextureHandler; }

};
