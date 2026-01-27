#include <core/Game.h>
#include <string.h>

Game::Game()
	:Window(nullptr),
	Renderer(nullptr),
	WorldContext(nullptr),
	Quit(false)
{
	if (!SDL_Init(SDL_INIT_VIDEO)) {
		SDL_Log("Failed to initialize SDL: %s", SDL_GetError());
		return;
	}

	Window = SDL_CreateWindow("Running Gun", 800, 600, 0);
	if (!Window) {
		SDL_Log("Failed to create window: %s", SDL_GetError());
		return;
	}

	Renderer = SDL_CreateRenderer(Window, nullptr);
	if (!Renderer) {
		SDL_Log("Failed to create renderer: %s", SDL_GetError());
		return;
	}

	WorldContext = new World(Renderer);

	std::unique_ptr<ResourceHandler> _textureHandler(new ResourceHandler(Renderer));
	std::unique_ptr<Physics> _physicsManager(new Physics());
	EnvironmentContext.SetTextureHandler(std::move(_textureHandler));
	EnvironmentContext.SetPhysics(std::move(_physicsManager));
	EnvironmentContext.SetWorld(WorldContext);
}

Game::~Game()
{
	delete WorldContext;
	if (Renderer) {
		SDL_DestroyRenderer(Renderer);
	}
	if (Window) {
		SDL_DestroyWindow(Window);
	}
	SDL_Quit();
}

void Game::Run()
{
	Uint64 _lastTime = SDL_GetPerformanceCounter();
	Uint64 _frequency = SDL_GetPerformanceFrequency();

	WorldContext->Init();
	WorldContext->BuildScene();

	while (!Quit) {
		// Calculate delta time
		Uint64 _currentTime = SDL_GetPerformanceCounter();
		float _deltaTime = static_cast<float>(_currentTime - _lastTime) / static_cast<float>(_frequency);
		_lastTime = _currentTime;

		// Begin input frame
		InputManagerContext.BeginFrame();

		// Process events
		SDL_Event _event;
		while (SDL_PollEvent(&_event)) {
			if (_event.type == SDL_EVENT_QUIT) {
				Quit = true;
			}
			InputManagerContext.ProcessEvent(_event);
		}

		// End input frame
		InputManagerContext.EndFrame();

		EnvironmentContext.SetDeltaTime(_deltaTime);

		// Clear screen
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);

		// Update and render
		WorldContext->Start();
		WorldContext->Update();
		WorldContext->PostUpdate();
		WorldContext->Render();

		// Present
		SDL_RenderPresent(Renderer);

		// Frame rate limiting (approximately 120 FPS)
		SDL_Delay(8);
	}
}
