#pragma once

#include <core/engine/framework/ISystem.h>

class GameServiceHost;
class ActorService;

// Runs the update loop on all actors tracked by ActorService.
// Handles: FlushAddQueue -> Update -> FlushRemoveQueue
class ActorSystem final : public ISystem
{
public:
    explicit ActorSystem(GameServiceHost& services);

    void Init() override;
    void Update() override;

private:
    ActorService& Actors;
};
