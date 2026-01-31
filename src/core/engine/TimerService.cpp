#include <core/engine/TimerService.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/RunnerService.h>
#include <algorithm>

TimerService::TimerService(GameServiceHost& services)
	: Services(services)
{
}

TimerHandle TimerService::ScheduleTimer(float delay, std::function<void()> callback)
{
	TimerHandle handle = NextTimerHandle++;
	float currentTime = Services.Get<RunnerService>().GetElapsedTime();
	float endTime = currentTime + delay;
	Timers.push_back(std::make_unique<Timer>(handle, endTime, std::move(callback)));
	return handle;
}

void TimerService::CancelTimer(TimerHandle handle)
{
	Timers.erase(
		std::remove_if(Timers.begin(), Timers.end(),
			[handle](const std::unique_ptr<Timer>& timer) { return timer->GetHandle() == handle; }),
		Timers.end());
}

void TimerService::Reset()
{
	Timers.clear();
	NextTimerHandle = 1;
}
