#pragma once

#include <functional>
#include <memory>
#include <rapidjson/document.h>
#include <string>
#include <unordered_map>

class Component;
class Entity;
class GameContext;

class PrefabComponentRegistry
{
public:
	using Factory = std::function<std::unique_ptr<Component>(Entity&, GameContext&, const rapidjson::Value&)>;

	void Register(const std::string& _type, Factory _factory);
	const Factory* Find(const std::string& _type) const;

private:
	std::unordered_map<std::string, Factory> Factories;
};
