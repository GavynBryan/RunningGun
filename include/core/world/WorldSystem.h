#pragma once

#include <core/engine/framework/ISystem.h>

class GameServiceHost;
class ActorService;
class WorldService;

// Coordinates actor updates with world-level state updates (camera tracking).
// Delegates actor iteration to ActorSystem, handles camera and game mode updates.
class WorldSystem final : public ISystem
{
public:
	explicit WorldSystem(GameServiceHost& services);

	void Init() override;
	void Update() override;

private:
	GameServiceHost& Services;
	WorldService& World;
};
