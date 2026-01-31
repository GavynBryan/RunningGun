#include <core/systems/TimerSystem.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/RunnerService.h>
#include <core/engine/TimerService.h>

TimerSystem::TimerSystem(GameServiceHost& services)
	: Services(services)
{
}

void TimerSystem::Update()
{
	auto& runner = Services.Get<RunnerService>();
	auto& timers = Services.Get<TimerService>();

	float currentTime = runner.GetElapsedTime();
	auto& timerList = timers.GetTimers();

	for (auto it = timerList.begin(); it != timerList.end(); ) {
		if ((*it)->IsExpired(currentTime)) {
			(*it)->Fire();
			it = timerList.erase(it);
		} else {
			++it;
		}
	}
}
