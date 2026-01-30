#pragma once

#include <string>

class PrefabComponentRegistry;
class PrefabLibrary;
class ResourceHandler;

class PrefabStore
{
public:
	PrefabStore();
	~PrefabStore();

	PrefabComponentRegistry& GetRegistry();
	PrefabLibrary& GetLibrary();
	bool LoadDefinitions(ResourceHandler& _textures, const std::string& _path);

private:
	PrefabLibrary* Library;
	PrefabComponentRegistry* Registry;
};
