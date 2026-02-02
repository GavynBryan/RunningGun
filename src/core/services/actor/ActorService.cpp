#include <core/actor/ActorService.h>
#include <core/engine/framework/GameServiceHost.h>
#include <algorithm>

const std::vector<Actor*> ActorService::EmptyActorList;

ActorService::ActorService() = default;

ActorService::~ActorService()
{
    // Unsubscribe from all actors to be safe (actors may outlive service in edge cases)
    for (auto& [actor, handle] : ComponentChangeHandles) {
        actor->OnComponentsChanged.Unsubscribe(handle);
    }
}

void ActorService::Update()
{
    RebuildCachesIfDirty();
}

void ActorService::AddActor(std::unique_ptr<Actor> actor)
{
    Actor* rawPtr = actor.get();

    // Subscribe to component changes
    DelegateHandle handle = actor->OnComponentsChanged.Subscribe([this]() {
        MarkCachesDirty();
    });
    ComponentChangeHandles[rawPtr] = handle;

    AddQueue.push_back(std::move(actor));
    CachesDirty = true;
}

void ActorService::RemoveActor(Actor* actor)
{
    if (!actor) {
        return;
    }

    // Recursively mark children for removal first
    // Copy children list since removal modifies it
    std::vector<Actor*> children = actor->GetChildren();
    for (Actor* child : children) {
        RemoveActor(child);
    }

    // Unsubscribe from component changes
    auto it = ComponentChangeHandles.find(actor);
    if (it != ComponentChangeHandles.end()) {
        actor->OnComponentsChanged.Unsubscribe(it->second);
        ComponentChangeHandles.erase(it);
    }

    RemoveSet.insert(actor);
    CachesDirty = true;
}

void ActorService::FlushAddQueue()
{
    for (auto& actor : AddQueue) {
        actor->Start();
        Actors.push_back(std::move(actor));
    }
    AddQueue.clear();
}

void ActorService::FlushRemoveQueue()
{
    if (RemoveSet.empty()) {
        return;
    }

    Actors.erase(
        std::remove_if(Actors.begin(), Actors.end(),
            [this](const Actor::Ptr& actor) {
                return RemoveSet.find(actor.get()) != RemoveSet.end();
            }),
        Actors.end()
    );
    RemoveSet.clear();
}

void ActorService::ClearAll()
{
    // Unsubscribe from all actors
    for (auto& [actor, handle] : ComponentChangeHandles) {
        actor->OnComponentsChanged.Unsubscribe(handle);
    }
    ComponentChangeHandles.clear();

    Actors.clear();
    AddQueue.clear();
    RemoveSet.clear();
    CachesDirty = true;
}

const std::vector<Actor*>& ActorService::GetActorsByTag(ACTOR_TAG tag) const
{
    auto it = TagCache.find(tag);
    if (it != TagCache.end()) {
        return it->second;
    }
    return EmptyActorList;
}

Actor* ActorService::GetFirstActorByTag(ACTOR_TAG tag) const
{
    const auto& actors = GetActorsByTag(tag);
    return actors.empty() ? nullptr : actors.front();
}

void ActorService::RebuildCachesIfDirty()
{
    if (!CachesDirty) {
        return;
    }

    RebuildTagCache();
    RebuildComponentCache();
    CachesDirty = false;
}

void ActorService::RebuildTagCache()
{
    TagCache.clear();
    for (const auto& actor : Actors) {
        TagCache[actor->GetTag()].push_back(actor.get());
    }
}

void ActorService::RebuildComponentCache()
{
    // Clear all cached component queries - they'll be rebuilt lazily on next access
    ComponentCache.clear();
}
