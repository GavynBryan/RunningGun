#pragma once

#include <core/Timer.h>
#include <functional>
#include <memory>
#include <vector>

class GameServiceHost;

class TimerService final
{
public:
	explicit TimerService(GameServiceHost& services);

	TimerHandle ScheduleTimer(float delay, std::function<void()> callback);
	void CancelTimer(TimerHandle handle);
	void Reset();

	std::vector<std::unique_ptr<Timer>>& GetTimers() { return Timers; }
	const std::vector<std::unique_ptr<Timer>>& GetTimers() const { return Timers; }

private:
	GameServiceHost& Services;
	std::vector<std::unique_ptr<Timer>> Timers;
	TimerHandle NextTimerHandle = 1;
};
