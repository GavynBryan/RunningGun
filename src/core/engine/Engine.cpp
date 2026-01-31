#include <core/engine/Engine.h>
#include <core/GameMode.h>
#include <core/ResourceHandler.h>
#include <core/Camera.h>
#include <core/engine/InputService.h>
#include <core/engine/ObjectPoolService.h>
#include <core/engine/PhysicsService.h>
#include <core/engine/RenderService.h>
#include <core/engine/RunnerService.h>
#include <core/engine/SystemOrder.h>
#include <core/engine/TimerService.h>
#include <core/engine/WorldService.h>
#include <core/systems/RunnerSystem.h>
#include <core/systems/TimerSystem.h>
#include <core/systems/PhysicsSystem.h>
#include <core/systems/WorldSystem.h>
#include <core/systems/PoolMaintenanceSystem.h>

Engine::Engine()
	: Window(nullptr),
	Renderer(nullptr),
	Quit(false),
	Systems(Services)
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

	// Register services (pure data/context objects)
	Services.AddService<RunnerService>();
	Services.AddService<TimerService>(Services);
	Services.AddService<InputService>(InputManagerContext);
	Services.AddService<PhysicsService>();
	Services.AddService<RenderService>(Renderer, std::make_unique<ResourceHandler>(Renderer), std::make_unique<Camera>(800.0f, 600.0f));
	Services.AddService<WorldService>(Services);
	Services.AddService<ObjectPoolService>(Services, Prefabs);

	// Register systems (logic)
	Systems.AddSystem<RunnerSystem>(SystemOrder::Runner);
	Systems.AddSystem<TimerSystem>(SystemOrder::Timer);
	Systems.AddSystem<PhysicsSystem>(SystemOrder::Physics);
	Systems.AddSystem<WorldSystem>(SystemOrder::World);
	Systems.AddSystem<PoolMaintenanceSystem>(SystemOrder::PoolMaintenance);

	Prefabs.SetServices(Services);
}

Engine::~Engine()
{
	Systems.Shutdown();

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
	Systems.Init();

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

		// Update systems
		Systems.Update();

		// Present
		SDL_RenderPresent(Renderer);

		// Frame rate limiting (approximately 120 FPS)
		SDL_Delay(8);
	}
}

World& Engine::GetWorld()
{
	return Services.Get<WorldService>().GetWorld();
}

GameServiceHost& Engine::GetServices()
{
	return Services;
}

SystemHost& Engine::GetSystems()
{
	return Systems;
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
