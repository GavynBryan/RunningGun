#pragma once

#include <SDL3/SDL.h>

class RunnerService final
{
public:
	float GetDeltaTime() const { return DeltaTime; }
	float GetElapsedTime() const { return ElapsedTime; }
	Uint64 GetLastTime() const { return LastTime; }
	Uint64 GetFrequency() const { return Frequency; }
	bool IsStarted() const { return Started; }

	void SetDeltaTime(float dt) { DeltaTime = dt; }
	void SetElapsedTime(float time) { ElapsedTime = time; }
	void SetLastTime(Uint64 time) { LastTime = time; }
	void SetFrequency(Uint64 freq) { Frequency = freq; }
	void SetStarted(bool started) { Started = started; }

	void ResetClock()
	{
		LastTime = SDL_GetPerformanceCounter();
		ElapsedTime = 0.0f;
		DeltaTime = 0.0f;
	}

private:
	Uint64 LastTime = 0;
	Uint64 Frequency = 0;
	float DeltaTime = 0.0f;
	float ElapsedTime = 0.0f;
	bool Started = false;
};
