#pragma once

#include <core/framework/ISystem.h>

class GameServiceHost;
class TimeService;

class TimerSystem final : public ISystem
{
public:
	explicit TimerSystem(GameServiceHost& services);

private:
	void Update() override;

	TimeService& Time;
};
