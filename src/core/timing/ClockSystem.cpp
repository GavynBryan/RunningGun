#include <core/timing/ClockSystem.h>
#include <core/framework/GameServiceHost.h>
#include <core/timing/TimeService.h>

ClockSystem::ClockSystem(GameServiceHost& services)
	: Time(services.Get<TimeService>())
{
}

void ClockSystem::Init()
{
	Time.StartClock();
}

void ClockSystem::Update()
{
	Time.TickClock();
}
