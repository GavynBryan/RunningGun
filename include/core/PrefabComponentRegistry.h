#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class Component;
class Entity;
class GameContext;

class PrefabComponentRegistry
{
public:
	using Factory = std::function<std::unique_ptr<Component>(Entity&, GameContext&, std::string_view)>;

	void Register(const std::string& _type, Factory _factory);
	const Factory* Find(const std::string& _type) const;
	bool Contains(const std::string& _type) const;
	size_t Count() const;

private:
	std::unordered_map<std::string, Factory> Factories;
};
