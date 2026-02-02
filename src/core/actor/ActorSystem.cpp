#include <core/actor/ActorSystem.h>
#include <core/engine/framework/GameServiceHost.h>
#include <core/actor/ActorService.h>

ActorSystem::ActorSystem(GameServiceHost& services)
    : Actors(services.Get<ActorService>())
{
}

void ActorSystem::Init()
{
    // Initial flush of any actors added during scene building
    Actors.FlushAddQueue();
}

void ActorSystem::Update()
{
    // Process actors queued for addition
    Actors.FlushAddQueue();

    // Update all active actors
    for (const auto& actor : Actors.GetActors()) {
        if (actor->IsEnabled()) {
            actor->Update();
        }
    }

    // Process actors queued for removal
    Actors.FlushRemoveQueue();
}
