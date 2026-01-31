#pragma once

#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

class Entity;
class GameServiceHost;
class PrefabSystem;
class World;

class ObjectPoolService final
{
public:
	static constexpr size_t DefaultPoolSize = 8;
	static constexpr float MaintenanceIntervalSeconds = 60.0f;

	ObjectPoolService(GameServiceHost& services, PrefabSystem& prefabs);

	Entity* FetchPrefab(std::string_view prefabId);
	void ClearPools();

	struct Pool
	{
		std::string PrefabId;
		size_t Size = 0;
		std::vector<Entity*> Entries;
		float NextMaintenanceTime = 0.0f;
	};

	std::unordered_map<std::string, Pool>& GetPools() { return Pools; }
	const std::unordered_map<std::string, Pool>& GetPools() const { return Pools; }
	PrefabSystem& GetPrefabs() { return Prefabs; }

	Pool& GetOrCreatePool(std::string_view prefabId);
	bool RunMaintenance(Pool& pool);

private:
	GameServiceHost& Services;
	PrefabSystem& Prefabs;
	std::unordered_map<std::string, Pool> Pools;

	Entity* AcquireFromPool(Pool& pool);
	void EnsurePoolSize(Pool& pool, size_t newSize);
};
