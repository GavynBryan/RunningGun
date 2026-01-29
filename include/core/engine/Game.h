#pragma once
#include <SDL3/SDL.h>
#include <memory>
#include <core/engine/GameContext.h>
#include <core/Physics.h>
#include <core/InputManager.h>

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
