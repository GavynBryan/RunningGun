#pragma once
#include <SDL3/SDL.h>
#include "Environment.h"
#include "Physics.h"
#include "InputManager.h"

class Game
{
private:
	SDL_Window*								Window;
	SDL_Renderer*							Renderer;
	bool									Quit;

	World*									WorldContext;

private:
	Environment								EnvironmentContext;
	InputManager							InputManagerContext;

public:
	void	Run();
	void	QuitGame() { Quit = true; }

	SDL_Renderer* GetRenderer() { return Renderer; }

	Game();
	~Game();
};
