#pragma once
#include "ResourceHandler.h"
#include "World.h"

class Locator
{
private:
	static ResourceHandler* TextureHandler;

public:
	static void Provide(ResourceHandler* _textures) {
		TextureHandler = _textures;
	}

	static ResourceHandler* GetTextureHandler() { return TextureHandler; }

};
