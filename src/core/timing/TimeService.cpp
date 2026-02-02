#include <core/timing/TimeService.h>
#include <algorithm>

// ===== Clock Methods =====

void TimeService::StartClock()
{
	if (Started) {
		return;
	}
	Frequency = SDL_GetPerformanceFrequency();
	LastTime = SDL_GetPerformanceCounter();
	DeltaTimeValue = 0.0f;
	ElapsedTime = 0.0f;
	Started = true;
}

void TimeService::TickClock()
{
	if (!Started) {
		StartClock();
		return;
	}

	Uint64 currentTime = SDL_GetPerformanceCounter();
	DeltaTimeValue = static_cast<float>(currentTime - LastTime) / static_cast<float>(Frequency);
	LastTime = currentTime;
	ElapsedTime += DeltaTimeValue;
}

void TimeService::ResetClock()
{
	LastTime = SDL_GetPerformanceCounter();
	ElapsedTime = 0.0f;
	DeltaTimeValue = 0.0f;
}

// ===== Timer Methods =====

void TimeService::ProcessTimers(float currentTime)
{
	for (auto it = Timers.begin(); it != Timers.end(); ) {
		if ((*it)->IsExpired(currentTime)) {
			(*it)->Fire();
			it = Timers.erase(it);
		} else {
			++it;
		}
	}
}

TimerHandle TimeService::ScheduleTimer(float delay, std::function<void()> callback)
{
	TimerHandle handle = NextTimerHandle++;
	float endTime = ElapsedTime + delay;
	Timers.push_back(std::make_unique<Timer>(handle, endTime, std::move(callback)));
	return handle;
}

void TimeService::CancelTimer(TimerHandle handle)
{
	Timers.erase(
		std::remove_if(Timers.begin(), Timers.end(),
			[handle](const std::unique_ptr<Timer>& timer) { return timer->GetHandle() == handle; }),
		Timers.end());
}

void TimeService::ResetTimers()
{
	Timers.clear();
	NextTimerHandle = 1;
}
