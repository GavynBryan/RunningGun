#pragma once
#include <memory>

class Entity;
class GameContext;

class Prefabs
{
public:
	Prefabs();
	~Prefabs();

	static std::unique_ptr<Entity>	GetPlayer(GameContext& _context);
	static std::unique_ptr<Entity>	GetBull(GameContext& _context);
	static std::unique_ptr<Entity>	GetScorpion(GameContext& _context);
};
