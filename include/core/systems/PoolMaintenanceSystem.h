#pragma once

class GameServiceHost;

class PoolMaintenanceSystem final
{
public:
	explicit PoolMaintenanceSystem(GameServiceHost& services);

	void Update();
	void Shutdown();

private:
	GameServiceHost& Services;
};
