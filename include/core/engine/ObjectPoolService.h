#pragma once

#include <core/engine/IService.h>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

class Entity;
class PrefabSystem;

class ObjectPoolService final : public IService
{
public:
	static constexpr size_t DefaultPoolSize = 8;
	static constexpr float MaintenanceIntervalSeconds = 60.0f;

	explicit ObjectPoolService(PrefabSystem& prefabs);

	void Init() override;
	void Update() override;
	void Shutdown() override;

	Entity* FetchPrefab(std::string_view prefabId);
	void ClearPools();

private:
	struct Pool
	{
		std::string PrefabId;
		size_t Size = 0;
		std::vector<Entity*> Entries;
		float NextMaintenanceTime = 0.0f;
	};

	PrefabSystem& Prefabs;
	std::unordered_map<std::string, Pool> Pools;
	Pool& GetOrCreatePool(std::string_view prefabId);
	Entity* AcquireFromPool(Pool& pool);
	void EnsurePoolSize(Pool& pool, size_t newSize);
	bool RunMaintenance(Pool& pool);
};
