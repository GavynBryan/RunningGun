#pragma once

class GameServiceHost;

class TimerSystem final
{
public:
	explicit TimerSystem(GameServiceHost& services);

	void Update();

private:
	GameServiceHost& Services;
};
