#pragma once

class GameServiceHost;

class WorldSystem final
{
public:
	explicit WorldSystem(GameServiceHost& services);

	void Init();
	void Update();

	void ResetScene();

private:
	GameServiceHost& Services;

	void InitScene();
};
