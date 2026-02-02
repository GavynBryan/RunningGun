#pragma once

#include <core/framework/IService.h>
#include <unordered_map>
#include <string>
#include <string_view>
#include <vector>

class Actor;
class PrefabService;

class ObjectPoolService final : public IService
{
public:
	static constexpr size_t DefaultPoolSize = 8;
	static constexpr float MaintenanceIntervalSeconds = 60.0f;

	ObjectPoolService() = default;
	~ObjectPoolService() { ClearPools(); }

	Actor* FetchPrefab(std::string_view prefabId);
	void ClearPools();

private:
	struct Pool
	{
		std::string PrefabId;
		size_t Size = 0;
		std::vector<Actor*> Entries;
		float NextMaintenanceTime = 0.0f;
	};

	std::unordered_map<std::string, Pool> Pools;
	Pool& GetOrCreatePool(std::string_view prefabId);
	Actor* AcquireFromPool(Pool& pool);
	void EnsurePoolSize(Pool& pool, size_t newSize);
	bool RunMaintenance(Pool& pool);
};
