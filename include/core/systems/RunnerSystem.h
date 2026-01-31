#pragma once

class GameServiceHost;

class RunnerSystem final
{
public:
	explicit RunnerSystem(GameServiceHost& services);

	void Init();
	void Update();

private:
	GameServiceHost& Services;
};
