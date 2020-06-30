#include <core/Game.h>
#include <string.h>

Game::Game()
	:mWindow(sf::VideoMode(800, 600), "Running Gun"),
	mWorld(mWindow),
	quit(false)
{
	std::unique_ptr<ResourceHandler<sf::Texture>>	_TextureHandler(new ResourceHandler<sf::Texture>());
	std::unique_ptr<sPhysics>						_PhysicsManager(new sPhysics());
	mEnvironment.setTextureHandler(std::move(_TextureHandler));
	mEnvironment.setPhysics(std::move(_PhysicsManager));
	mEnvironment.setWorld(&mWorld);
}

Game::~Game()
{
}

void Game::run()
{
	sf::Clock	deltaClock;
	mWindow.setFramerateLimit(120);
	mWorld.init();
	mWorld.buildScene();
	while (mWindow.isOpen() && !quit) {
		//get delta time
		float dt = deltaClock.restart().asSeconds();
		sf::Event event;
		while (mWindow.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				mWindow.close();
			}
		}
		mEnvironment.setDeltaTime(dt);
		mWindow.clear();
		mWorld.start();
		mWorld.update();
		mWorld.postUpdate();
		mWorld.render();
		mWindow.display();

	}
}
