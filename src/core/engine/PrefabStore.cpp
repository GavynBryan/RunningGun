#include <core/engine/PrefabStore.h>
#include <core/PrefabComponentRegistry.h>
#include <core/PrefabLibrary.h>
#include <core/PrefabLoader.h>
#include <core/ResourceHandler.h>

PrefabStore::PrefabStore()
	: Library(new PrefabLibrary())
	, Registry(new PrefabComponentRegistry())
{
}

PrefabStore::~PrefabStore()
{
	delete Registry;
	delete Library;
}

PrefabComponentRegistry& PrefabStore::GetRegistry()
{
	return *Registry;
}

PrefabLibrary& PrefabStore::GetLibrary()
{
	return *Library;
}

bool PrefabStore::LoadDefinitions(ResourceHandler& _textures, const std::string& _path)
{
	if (!PrefabLoader::LoadFromFile(_path, *Library)) {
		return false;
	}

	for (const auto& _texture : Library->GetTexturePaths()) {
		_textures.Load(_texture);
	}
	return true;
}
