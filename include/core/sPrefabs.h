#pragma once
#include <memory>

class Entity;

class sPrefabs
{
public:
	sPrefabs();
	~sPrefabs();

	static std::unique_ptr<Entity>	getPlayer();
	static std::unique_ptr<Entity>	getBull();
	static std::unique_ptr<Entity>	getScorpion();
};

