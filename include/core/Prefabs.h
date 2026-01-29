#pragma once
#include <memory>
#include <string>

class Entity;
class GameContext;
class PrefabComponentRegistry;
class PrefabLibrary;

class Prefabs
{
public:
	Prefabs();
	~Prefabs();

	static bool LoadDefinitions(GameContext& _context, const std::string& _path);
	static void RegisterDefaultComponents();
	static PrefabComponentRegistry& GetComponentRegistry();
	static PrefabLibrary& GetLibrary();

	static std::unique_ptr<Entity>	GetPlayer(GameContext& _context);
	static std::unique_ptr<Entity>	GetBull(GameContext& _context);
	static std::unique_ptr<Entity>	GetScorpion(GameContext& _context);
};
