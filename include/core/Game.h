#pragma once
#include <SDL3/SDL.h>
#include "sEnvironment.h"
#include "sPhysics.h"
#include "InputManager.h"

class Game
{
private:
	SDL_Window*								mWindow;
	SDL_Renderer*							mRenderer;
	bool									quit;

	World*									mWorld;

private:
	sEnvironment							mEnvironment;
	InputManager							mInputManager;

public:
	void	run();
	void	quitGame() { quit = true; }

	SDL_Renderer* getRenderer() { return mRenderer; }

	Game();
	~Game();
};
