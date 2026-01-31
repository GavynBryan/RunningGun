#include <core/systems/RunnerSystem.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/RunnerService.h>
#include <SDL3/SDL.h>

RunnerSystem::RunnerSystem(GameServiceHost& services)
	: Services(services)
{
}

void RunnerSystem::Init()
{
	auto& runner = Services.Get<RunnerService>();
	runner.SetFrequency(SDL_GetPerformanceFrequency());
	runner.SetLastTime(SDL_GetPerformanceCounter());
	runner.SetDeltaTime(0.0f);
	runner.SetElapsedTime(0.0f);
	runner.SetStarted(true);
}

void RunnerSystem::Update()
{
	auto& runner = Services.Get<RunnerService>();

	if (!runner.IsStarted()) {
		Init();
		return;
	}

	Uint64 currentTime = SDL_GetPerformanceCounter();
	float deltaTime = static_cast<float>(currentTime - runner.GetLastTime()) / static_cast<float>(runner.GetFrequency());
	runner.SetLastTime(currentTime);
	runner.SetDeltaTime(deltaTime);
	runner.SetElapsedTime(runner.GetElapsedTime() + deltaTime);
}
