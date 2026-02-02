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
class ActorService final : public IService
{
public:
    ActorService();
    ~ActorService();

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

    // --- Tag-based queries ---
    std::vector<Actor*> GetActorsByTag(ACTOR_TAG tag) const;
    Actor* GetFirstActorByTag(ACTOR_TAG tag) const;

    // --- Component-based queries (cached) ---
    template <typename Comp>
    std::vector<Actor*> GetActorsWithComponent() const;

    template <typename Comp>
    Actor* GetFirstActorWithComponent() const;

    // Rebuild component caches (call after bulk actor changes)
    void RebuildCaches();

    // Invalidate caches (called automatically when actors change)
    void InvalidateCaches() { CachesDirty = true; }

private:
    void EnsureCachesValid() const;
    void RebuildTagCache() const;
    void RebuildComponentCache() const;

    std::vector<Actor::Ptr> Actors;
    std::vector<Actor::Ptr> AddQueue;
    std::unordered_set<Actor*> RemoveSet;

    // Caches - mutable for lazy rebuild on const queries
    mutable bool CachesDirty = true;
    mutable std::unordered_map<ACTOR_TAG, std::vector<Actor*>> TagCache;
    mutable std::unordered_map<std::type_index, std::vector<Actor*>> ComponentCache;
};

// Template implementations

template <typename Comp>
std::vector<Actor*> ActorService::GetActorsWithComponent() const
{
    EnsureCachesValid();

    std::type_index typeIdx(typeid(Comp));
    auto it = ComponentCache.find(typeIdx);
    if (it != ComponentCache.end()) {
        return it->second;
    }

    // Not in cache yet - build for this component type
    std::vector<Actor*> result;
    for (const auto& actor : Actors) {
        if (actor->GetComponent<Comp>() != nullptr) {
            result.push_back(actor.get());
        }
    }
    ComponentCache[typeIdx] = result;
    return result;
}

template <typename Comp>
Actor* ActorService::GetFirstActorWithComponent() const
{
    auto actors = GetActorsWithComponent<Comp>();
    return actors.empty() ? nullptr : actors.front();
}
