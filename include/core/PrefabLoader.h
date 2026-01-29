#pragma once

#include <string>

class PrefabLibrary;

class PrefabLoader
{
public:
	static bool LoadFromFile(const std::string& _path, PrefabLibrary& _library);
};
