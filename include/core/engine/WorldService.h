#pragma once

#include <core/engine/IService.h>
#include <memory>

class GameMode;
class World;

class WorldService final : public IService
{
public:
	void Init() override;
	void Update() override;

	void SetGameMode(GameMode* mode);
	GameMode* GetGameMode() const { return Mode; }

	World& GetWorld() const;

	void ResetScene();

private:
	std::unique_ptr<World> WorldContext;
	GameMode* Mode = nullptr;
	bool SceneInitialized = false;
};
