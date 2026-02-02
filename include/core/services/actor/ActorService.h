#pragma once

#include <core/base/Actor.h>
#include <core/engine/framework/IService.h>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <typeindex>

class GameServiceHost;

// Tracks all actors in the game world with optional caching by Tag and Component type.
// Handles adding/removing actors via queues to avoid iterator invalidation.
// Subscribes to actor OnComponentsChanged for automatic cache invalidation.
class ActorService final : public IService
{
public:
    ActorService();
    ~ActorService();

    // Called each frame to rebuild caches if dirty
    void Update();

    // Queue an actor for addition (processed during FlushAddQueue)
    void AddActor(std::unique_ptr<Actor> actor);

    // Mark an actor for removal (processed during FlushRemoveQueue)
    void RemoveActor(Actor* actor);

    // Process queued adds - starts pending actors and moves to main list
    void FlushAddQueue();

    // Process queued removals
    void FlushRemoveQueue();

    // Clear all actors and queues
    void ClearAll();

    // Access all active actors
    const std::vector<Actor::Ptr>& GetActors() const { return Actors; }

    // --- Tag-based queries (use cached data) ---
    const std::vector<Actor*>& GetActorsByTag(ACTOR_TAG tag) const;
    Actor* GetFirstActorByTag(ACTOR_TAG tag) const;

    // --- Component-based queries (use cached data) ---
    template <typename Comp>
    const std::vector<Actor*>& GetActorsWithComponent();

    template <typename Comp>
    Actor* GetFirstActorWithComponent();

private:
    void RebuildCachesIfDirty();
    void RebuildTagCache();
    void RebuildComponentCache();
    void MarkCachesDirty() { CachesDirty = true; }

    std::vector<Actor::Ptr> Actors;
    std::vector<Actor::Ptr> AddQueue;
    std::unordered_set<Actor*> RemoveSet;

    // Subscription handles for component change events
    std::unordered_map<Actor*, DelegateHandle> ComponentChangeHandles;

    // Dirty flag - set by component change callback, cleared after rebuild
    bool CachesDirty = true;

    // Caches
    std::unordered_map<ACTOR_TAG, std::vector<Actor*>> TagCache;
    std::unordered_map<std::type_index, std::vector<Actor*>> ComponentCache;

    // Empty vector for returning references when no results
    static const std::vector<Actor*> EmptyActorList;
};

// Template implementations

template <typename Comp>
const std::vector<Actor*>& ActorService::GetActorsWithComponent()
{
    std::type_index typeIdx(typeid(Comp));
    auto it = ComponentCache.find(typeIdx);
    if (it != ComponentCache.end()) {
        return it->second;
    }

    // Not in cache yet - build for this component type
    std::vector<Actor*> result;
    result.reserve(Actors.size() / 4); // Reasonable initial capacity
    for (const auto& actor : Actors) {
        if (actor->GetComponent<Comp>() != nullptr) {
            result.push_back(actor.get());
        }
    }
    auto [insertedIt, _] = ComponentCache.emplace(typeIdx, std::move(result));
    return insertedIt->second;
}

template <typename Comp>
Actor* ActorService::GetFirstActorWithComponent()
{
    const auto& actors = GetActorsWithComponent<Comp>();
    return actors.empty() ? nullptr : actors.front();
}
{
    auto actors = GetActorsWithComponent<Comp>();
    return actors.empty() ? nullptr : actors.front();
}
