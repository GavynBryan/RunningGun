#pragma once

#include <memory>

class GameMode;
class GameServiceHost;
class World;

class WorldService final
{
public:
	explicit WorldService(GameServiceHost& services);

	void SetGameMode(GameMode* mode);
	GameMode* GetGameMode() const { return Mode; }

	World& GetWorld() const;

	bool IsSceneInitialized() const { return SceneInitialized; }
	void SetSceneInitialized(bool value) { SceneInitialized = value; }

private:
	std::unique_ptr<World> WorldContext;
	GameMode* Mode = nullptr;
	bool SceneInitialized = false;
};
