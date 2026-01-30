#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <core/GameMode.h>
#include <core/engine/EngineServices.h>
#include <core/events/MulticastDelegate.h>
#include <core/UIHealthBar.h>
#include <core/UIText.h>
#include <memory>
#include <string>

class ObjectPool;
class PlayerComponent;
class World;
class Entity;
class PrefabSystem;

class RunningGunGameMode : public GameMode
{
private:
	SDL_Renderer*				Renderer;
	EngineServices&				Services;
	PrefabSystem&				Prefabs;
	World&						WorldContext;

	UIHealthBar*				HealthBar;
	UIText*						StatusTextUI;
	TTF_Font*					GameFont;

	std::unique_ptr<ObjectPool>	ObjectPoolContext;
	Entity*						PlayerEntity;
	PlayerComponent*			PlayerComponentRef;

	float						SpawnScorpion1Interval;
	float						LastSpawn1Time;
	float						SpawnScorpion2Interval;
	float						LastSpawn2Time;

	bool						Win;
	bool						Lose;
	bool						ResetRequested;

	DelegateHandle				PlayerDiedHandle;
	DelegateHandle				BossDiedHandle;

	void						SetStatusText(const std::string& _text);
	void						SubscribeToEvents();
	void						UnsubscribeFromEvents();

public:
								RunningGunGameMode(SDL_Renderer* _renderer, EngineServices& _services, PrefabSystem& _prefabs, World& _world);
								~RunningGunGameMode() override;

	void						Init() override;
	void						BuildScene() override;
	void						Update() override;
	void						PostUpdate() override;
	void						OnWin(Entity* _boss);
	void						OnLose(Entity* _player);
	void						RequestReset() override;
};
