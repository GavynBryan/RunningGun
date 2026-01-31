#include <core/engine/ObjectPoolService.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/RunnerService.h>
#include <core/engine/WorldService.h>
#include <core/PrefabSystem.h>
#include <core/Entity.h>
#include <core/World.h>
#include <SDL3/SDL.h>

ObjectPoolService::ObjectPoolService(GameServiceHost& services, PrefabSystem& prefabs)
	: Services(services),
	Prefabs(prefabs)
{
}

Entity* ObjectPoolService::FetchPrefab(std::string_view prefabId)
{
	auto& pool = GetOrCreatePool(prefabId);
	auto* entity = AcquireFromPool(pool);
	if (!entity) {
		return nullptr;
	}
	if (!entity->IsEnabled()) {
		entity->Enable();
	}
	return entity;
}

void ObjectPoolService::ClearPools()
{
	Pools.clear();
}

ObjectPoolService::Pool& ObjectPoolService::GetOrCreatePool(std::string_view prefabId)
{
	auto key = std::string(prefabId);
	auto iter = Pools.find(key);
	if (iter != Pools.end()) {
		return iter->second;
	}

	float currentTime = Services.Get<RunnerService>().GetElapsedTime();

	Pool pool;
	pool.PrefabId = key;
	pool.NextMaintenanceTime = currentTime + MaintenanceIntervalSeconds;
	EnsurePoolSize(pool, DefaultPoolSize);
	auto inserted = Pools.emplace(key, std::move(pool));
	return inserted.first->second;
}

Entity* ObjectPoolService::AcquireFromPool(Pool& pool)
{
	for (auto* entity : pool.Entries) {
		if (entity && !entity->IsEnabled()) {
			return entity;
		}
	}

	return nullptr;
}

void ObjectPoolService::EnsurePoolSize(Pool& pool, size_t newSize)
{
	if (newSize <= pool.Size) {
		return;
	}

	const auto* definition = Prefabs.Find(pool.PrefabId);
	if (!definition) {
		SDL_Log("ObjectPoolService: Prefab '%s' not found.", pool.PrefabId.c_str());
		return;
	}

	auto& world = Services.Get<WorldService>().GetWorld();
	const size_t toCreate = newSize - pool.Size;
	for (size_t index = 0; index < toCreate; ++index) {
		auto entity = Prefabs.Instantiate(*definition);
		if (!entity) {
			continue;
		}
		entity->Disable();
		pool.Entries.push_back(entity.get());
		world.AddObject(std::move(entity));
		pool.Size += 1;
	}
}

bool ObjectPoolService::RunMaintenance(Pool& pool)
{
	if (pool.Size == 0) {
		return true;
	}

	size_t activeCount = 0;
	for (auto* entity : pool.Entries) {
		if (entity && entity->IsEnabled()) {
			++activeCount;
		}
	}

	const float halfSize = static_cast<float>(pool.Size) * 0.5f;
	if (activeCount >= halfSize) {
		return false;
	}

	if (pool.Size <= DefaultPoolSize) {
		pool.Entries.clear();
		pool.Size = 0;
		return true;
	}

	const size_t targetSize = pool.Size / 2;
	for (auto it = pool.Entries.begin(); it != pool.Entries.end() && pool.Size > targetSize; ) {
		if (*it && !(*it)->IsEnabled()) {
			it = pool.Entries.erase(it);
			--pool.Size;
		} else {
			++it;
		}
	}
	return false;
}
