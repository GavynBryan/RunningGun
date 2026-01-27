#include <core/Game.h>
#include <string.h>

Game::Game()
	:mWindow(nullptr),
	mRenderer(nullptr),
	mWorld(nullptr),
	quit(false)
{
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return;
	}

	mWindow = SDL_CreateWindow("Running Gun", 800, 600, 0);
	if (!mWindow) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return;
	}

	mRenderer = SDL_CreateRenderer(mWindow, nullptr);
	if (!mRenderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return;
	}

	mWorld = new World(mRenderer);

	std::unique_ptr<ResourceHandler>	_TextureHandler(new ResourceHandler(mRenderer));
	std::unique_ptr<sPhysics>			_PhysicsManager(new sPhysics());
	mEnvironment.setTextureHandler(std::move(_TextureHandler));
	mEnvironment.setPhysics(std::move(_PhysicsManager));
	mEnvironment.setWorld(mWorld);
}

Game::~Game()
{
	delete mWorld;
	if (mRenderer) {
		SDL_DestroyRenderer(mRenderer);
	}
	if (mWindow) {
		SDL_DestroyWindow(mWindow);
	}
	SDL_Quit();
}

void Game::run()
{
	Uint64 lastTime = SDL_GetPerformanceCounter();
	Uint64 frequency = SDL_GetPerformanceFrequency();

	mWorld->init();
	mWorld->buildScene();

	while (!quit) {
		// Calculate delta time
		Uint64 currentTime = SDL_GetPerformanceCounter();
		float dt = (float)(currentTime - lastTime) / (float)frequency;
		lastTime = currentTime;

		// Begin input frame
		mInputManager.beginFrame();

		// Process events
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				quit = true;
			}
			mInputManager.processEvent(event);
		}

		// End input frame
		mInputManager.endFrame();

		mEnvironment.setDeltaTime(dt);

		// Clear screen
		SDL_SetRenderDrawColor(mRenderer, 0, 0, 0, 255);
		SDL_RenderClear(mRenderer);

		// Update and render
		mWorld->start();
		mWorld->update();
		mWorld->postUpdate();
		mWorld->render();

		// Present
		SDL_RenderPresent(mRenderer);

		// Frame rate limiting (approximately 120 FPS)
		SDL_Delay(8);
	}
}
