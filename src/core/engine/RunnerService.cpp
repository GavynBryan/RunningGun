#include <core/engine/RunnerService.h>

void RunnerService::Init()
{
	Frequency = SDL_GetPerformanceFrequency();
	LastTime = SDL_GetPerformanceCounter();
	DeltaTimeValue = 0.0f;
	ElapsedTime = 0.0f;
	Started = true;
}

void RunnerService::Update()
{
	if (!Started) {
		Init();
		return;
	}

	Uint64 currentTime = SDL_GetPerformanceCounter();
	DeltaTimeValue = static_cast<float>(currentTime - LastTime) / static_cast<float>(Frequency);
	LastTime = currentTime;
	ElapsedTime += DeltaTimeValue;
}

void RunnerService::ResetClock()
{
	LastTime = SDL_GetPerformanceCounter();
	ElapsedTime = 0.0f;
	DeltaTimeValue = 0.0f;
}
