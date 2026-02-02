#pragma once

#include <core/timing/Timer.h>
#include <core/framework/IService.h>
#include <SDL3/SDL.h>
#include <functional>
#include <memory>
#include <vector>

class TimeService final : public IService
{
public:
	// ===== Clock State =====
	float GetDeltaTime() const { return DeltaTimeValue; }
	float GetElapsedTime() const { return ElapsedTime; }
	bool IsStarted() const { return Started; }

	// Clock modifiers (called by ClockSystem)
	void StartClock();
	void TickClock();
	void ResetClock();

	// ===== Timer State =====
	std::vector<std::unique_ptr<Timer>>& GetTimers() { return Timers; }
	const std::vector<std::unique_ptr<Timer>>& GetTimers() const { return Timers; }

	// Timer modifiers (called by TimerSystem)
	void ProcessTimers(float currentTime);

	// Timer public API
	TimerHandle ScheduleTimer(float delay, std::function<void()> callback);
	void CancelTimer(TimerHandle handle);
	void ResetTimers();

private:
	// Clock state
	Uint64 LastTime = 0;
	Uint64 Frequency = 0;
	float DeltaTimeValue = 0.0f;
	float ElapsedTime = 0.0f;
	bool Started = false;

	// Timer state
	std::vector<std::unique_ptr<Timer>> Timers;
	TimerHandle NextTimerHandle = 1;
};
