#pragma once
#include <memory>

class Entity;

class Prefabs
{
public:
	Prefabs();
	~Prefabs();

	static std::unique_ptr<Entity>	GetPlayer();
	static std::unique_ptr<Entity>	GetBull();
	static std::unique_ptr<Entity>	GetScorpion();
};
