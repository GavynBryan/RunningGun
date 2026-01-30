#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <core/engine/EngineServices.h>
#include <core/engine/PrefabStore.h>
#include <core/Physics.h>
#include <core/InputManager.h>

class World;
class GameMode;

class Engine
{
private:
	SDL_Window*								Window;
	SDL_Renderer*							Renderer;
	bool									Quit;

	World*									WorldContext;
	std::unique_ptr<GameMode>				Mode;

private:
	EngineServices							Services;
	InputManager							InputManagerContext;
	PrefabStore								Prefabs;

public:
	void	Run();
	void	QuitGame() { Quit = true; }

	SDL_Renderer* GetRenderer() { return Renderer; }
	World& GetWorld();
	EngineServices& GetServices();
	InputManager& GetInputManager();
	PrefabStore& GetPrefabs();

	void SetGameMode(std::unique_ptr<GameMode> _mode);

	Engine();
	~Engine();
};
