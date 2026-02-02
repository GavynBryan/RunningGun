#include <core/actor/ActorService.h>
#include <core/engine/framework/GameServiceHost.h>
#include <algorithm>

ActorService::ActorService() = default;
ActorService::~ActorService() = default;

void ActorService::AddActor(std::unique_ptr<Actor> actor)
{
    AddQueue.push_back(std::move(actor));
    InvalidateCaches();
}

void ActorService::RemoveActor(Actor* actor)
{
    if (actor) {
        RemoveSet.insert(actor);
        InvalidateCaches();
    }
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
    Actors.clear();
    AddQueue.clear();
    RemoveSet.clear();
    InvalidateCaches();
}

std::vector<Actor*> ActorService::GetActorsByTag(ACTOR_TAG tag) const
{
    EnsureCachesValid();

    auto it = TagCache.find(tag);
    if (it != TagCache.end()) {
        return it->second;
    }
    return {};
}

Actor* ActorService::GetFirstActorByTag(ACTOR_TAG tag) const
{
    auto actors = GetActorsByTag(tag);
    return actors.empty() ? nullptr : actors.front();
}

void ActorService::RebuildCaches()
{
    CachesDirty = true;
    EnsureCachesValid();
}

void ActorService::EnsureCachesValid() const
{
    if (!CachesDirty) {
        return;
    }

    RebuildTagCache();
    // ComponentCache is rebuilt lazily per-type in GetActorsWithComponent
    ComponentCache.clear();
    CachesDirty = false;
}

void ActorService::RebuildTagCache() const
{
    TagCache.clear();
    for (const auto& actor : Actors) {
        TagCache[actor->GetTag()].push_back(actor.get());
    }
}

void ActorService::RebuildComponentCache() const
{
    // This is called lazily from GetActorsWithComponent<T>()
    // Full rebuild not typically needed since we cache per-type on demand
    ComponentCache.clear();
}
