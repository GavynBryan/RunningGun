#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "GameMode.h"
#include "GameContext.h"
#include "UIHealthBar.h"
#include "UIText.h"
#include <memory>
#include <string>

class ObjectPool;
class PlayerComponent;
class World;
class Entity;

class RunningGunGameMode : public GameMode
{
private:
	SDL_Renderer*				Renderer;
	GameContext&				Context;
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
	bool						ResetRequested;

	void						SetStatusText(const std::string& _text);
public:
								RunningGunGameMode(SDL_Renderer* _renderer, GameContext& _context, World& _world);
								~RunningGunGameMode() override;

	void						Init() override;
	void						BuildScene() override;
	void						Update() override;
	void						PostUpdate() override;
	void						OnWin();
	void						RequestReset() override;
};
