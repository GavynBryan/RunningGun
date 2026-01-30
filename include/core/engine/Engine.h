#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <core/engine/GameServiceHost.h>
#include <core/PrefabSystem.h>
#include <core/InputManager.h>

class World;
class GameMode;

class Engine
{
private:
	SDL_Window*								Window;
	SDL_Renderer*							Renderer;
	bool									Quit;

	std::unique_ptr<GameMode>				Mode;

private:
	GameServiceHost							Services;
	InputManager							InputManagerContext;
	PrefabSystem							Prefabs;

public:
	void	Run();
	void	QuitGame() { Quit = true; }

	SDL_Renderer* GetRenderer() { return Renderer; }
	World& GetWorld();
	GameServiceHost& GetServices();
	InputManager& GetInputManager();
	PrefabSystem& GetPrefabs();

	void SetGameMode(std::unique_ptr<GameMode> _mode);

	Engine();
	~Engine();
};
