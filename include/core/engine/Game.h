#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include "GameContext.h"
#include "Physics.h"
#include "InputManager.h"

class World;
class GameMode;

class Game
{
private:
	SDL_Window*								Window;
	SDL_Renderer*							Renderer;
	bool									Quit;

	World*									WorldContext;
	std::unique_ptr<GameMode>				Mode;

private:
	GameContext								Context;
	InputManager							InputManagerContext;

public:
	void	Run();
	void	QuitGame() { Quit = true; }

	SDL_Renderer* GetRenderer() { return Renderer; }

	Game();
	~Game();
};
