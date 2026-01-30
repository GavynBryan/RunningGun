#include <core/engine/TimerService.h>
#include <core/engine/RunnerService.h>
#include <algorithm>

void TimerService::Update()
{
	float currentTime = GetHost().Get<RunnerService>().GetElapsedTime();
	for (auto it = Timers.begin(); it != Timers.end(); ) {
		if ((*it)->IsExpired(currentTime)) {
			(*it)->Fire();
			it = Timers.erase(it);
		} else {
			++it;
		}
	}
}

TimerHandle TimerService::ScheduleTimer(float delay, std::function<void()> callback)
{
	TimerHandle handle = NextTimerHandle++;
	float endTime = GetHost().Get<RunnerService>().GetElapsedTime() + delay;
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
