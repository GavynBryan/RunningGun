#pragma once
#include "sEnvironment.h"
#include "sPhysics.h"

class Game
{
private:
	sf::RenderWindow						mWindow;
	bool									quit;
	
	World									mWorld;
	
private:
	sEnvironment							mEnvironment;

public:
	void	run();
	void	quitGame() { quit = true; }

	Game();
	~Game();
	/*
	Game(const Game&) = delete;
	Game(Game&&) = delete;
	Game& operator=(const Game&) = delete;
	Game& operator=(Game&&) = delete;
	*/
};

