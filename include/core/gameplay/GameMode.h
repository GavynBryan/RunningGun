#pragma once

#include <core/framework/GameServiceHost.h>  // For dependency injection.

class GameMode {
public:
    explicit GameMode(GameServiceHost& services) : Services(services) {}
    virtual ~GameMode() = default;

    virtual void Init() = 0;
    virtual void BuildScene() = 0;
    virtual void Update() {}
    virtual void Shutdown() {}
    virtual void RequestReset() = 0;

protected:
    GameServiceHost& Services;
};
