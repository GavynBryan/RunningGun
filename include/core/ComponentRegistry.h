#pragma once

#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>

class Component;
class Entity;
class GameplayServices;

class ComponentRegistry
{
public:
	using Factory = std::function<std::unique_ptr<Component>(Entity&, GameplayServices&, std::string_view)>;

	void Register(std::string_view type, Factory factory);
	const Factory* Find(std::string_view type) const;
	bool Contains(std::string_view type) const;
	size_t Count() const;

private:
	std::unordered_map<std::string, Factory> Factories;
};
