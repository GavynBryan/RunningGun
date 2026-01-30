#pragma once

#include <core/engine/IService.h>
#include <SDL3/SDL.h>

class RunnerService final : public IService
{
public:
	void Init() override;
	void Update() override;

	float GetDeltaTime() const { return DeltaTimeValue; }
	float GetElapsedTime() const { return ElapsedTime; }

	void ResetClock();

private:
	Uint64 LastTime = 0;
	Uint64 Frequency = 0;
	float DeltaTimeValue = 0.0f;
	float ElapsedTime = 0.0f;
	bool Started = false;
};
