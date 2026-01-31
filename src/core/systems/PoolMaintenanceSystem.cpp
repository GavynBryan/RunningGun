#include <core/systems/PoolMaintenanceSystem.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/ObjectPoolService.h>
#include <core/engine/RunnerService.h>

PoolMaintenanceSystem::PoolMaintenanceSystem(GameServiceHost& services)
	: Services(services)
{
}

void PoolMaintenanceSystem::Update()
{
	auto& runner = Services.Get<RunnerService>();
	auto& pools = Services.Get<ObjectPoolService>();

	const float currentTime = runner.GetElapsedTime();
	auto& poolMap = pools.GetPools();

	for (auto it = poolMap.begin(); it != poolMap.end(); ) {
		auto& pool = it->second;
		if (pool.NextMaintenanceTime <= 0.0f) {
			pool.NextMaintenanceTime = currentTime + ObjectPoolService::MaintenanceIntervalSeconds;
		}
		while (currentTime >= pool.NextMaintenanceTime) {
			if (pools.RunMaintenance(pool)) {
				break;
			}
			pool.NextMaintenanceTime += ObjectPoolService::MaintenanceIntervalSeconds;
		}
		if (pool.Size == 0) {
			it = poolMap.erase(it);
		} else {
			++it;
		}
	}
}

void PoolMaintenanceSystem::Shutdown()
{
	Services.Get<ObjectPoolService>().ClearPools();
}
