#pragma once

#include <core/framework/ISystem.h>

class GameServiceHost;
class TimeService;

class ClockSystem final : public ISystem
{
public:
	explicit ClockSystem(GameServiceHost& services);

private:
	void Init() override;
	void Update() override;

	TimeService& Time;
};
