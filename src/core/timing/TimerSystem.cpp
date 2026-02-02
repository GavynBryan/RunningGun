#include <core/timing/TimerSystem.h>
#include <core/framework/GameServiceHost.h>
#include <core/timing/TimeService.h>

TimerSystem::TimerSystem(GameServiceHost& services)
	: Time(services.Get<TimeService>())
{
}

void TimerSystem::Update()
{
	Time.ProcessTimers(Time.GetElapsedTime());
}
