#include <core/engine/Engine.h>
#include <core/GameMode.h>
#include <core/ResourceHandler.h>
#include <core/Camera.h>
#include <core/engine/InputService.h>
#include <core/engine/ObjectPoolService.h>
#include <core/engine/PhysicsService.h>
#include <core/engine/RenderService.h>
#include <core/engine/RunnerService.h>
#include <core/engine/TimerService.h>
#include <core/engine/WorldService.h>

Engine::Engine()
	:Window(nullptr),
	Renderer(nullptr),
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

	Services.AddService<RunnerService>(0);
	Services.AddService<TimerService>(10);
	Services.AddService<InputService>(20, InputManagerContext);
	Services.AddService<PhysicsService>(30);
	Services.AddService<RenderService>(40, Renderer, std::make_unique<ResourceHandler>(Renderer), std::make_unique<Camera>(800.0f, 600.0f));
	Services.AddService<WorldService>(50);
	Services.AddService<ObjectPoolService>(60, Prefabs);

	Prefabs.SetServices(Services);
}

Engine::~Engine()
{
	if (Renderer) {
		SDL_DestroyRenderer(Renderer);
	}
	if (Window) {
		SDL_DestroyWindow(Window);
	}
	SDL_Quit();
}

void Engine::Run()
{
	if (!Mode) {
		SDL_Log("Engine cannot run without a game mode.");
		return;
	}
	Services.Init();

	while (!Quit) {
		// Begin input frame
		auto& inputService = Services.Get<InputService>();
		inputService.BeginFrame();

		// Process events
		SDL_Event _event;
		while (SDL_PollEvent(&_event)) {
			if (_event.type == SDL_EVENT_QUIT) {
				Quit = true;
			}
			inputService.ProcessEvent(_event);
		}

		// End input frame
		inputService.EndFrame();

		// Clear screen
		SDL_SetRenderDrawColor(Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Renderer);

		// Update and render
		Services.Update();

		// Present
		SDL_RenderPresent(Renderer);

		// Frame rate limiting (approximately 120 FPS)
		SDL_Delay(8);
	}

	Services.Shutdown();
}

World& Engine::GetWorld()
{
	return Services.Get<WorldService>().GetWorld();
}

GameServiceHost& Engine::GetServices()
{
	return Services;
}

InputManager& Engine::GetInputManager()
{
	return InputManagerContext;
}

PrefabSystem& Engine::GetPrefabs()
{
	return Prefabs;
}

void Engine::SetGameMode(std::unique_ptr<GameMode> _mode)
{
	Mode = std::move(_mode);
	Services.Get<WorldService>().SetGameMode(Mode.get());
}
