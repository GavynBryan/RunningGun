#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Entity.h"
#include "GameContext.h"
#include "QuadTree.h"
#include "Timer.h"
#include "UIManager.h"
#include "UIHealthBar.h"
#include "UIText.h"
#include <memory>

class ObjectPool;
class PlayerComponent;

class World
{
private:
	void						HandleQueue();

private:
	SDL_Renderer*				Renderer;
	Sprite						Background;
	GameContext&				Context;

	std::vector<Entity::Ptr>	Entities;
	std::vector<Entity::Ptr>	AddQueue;

	Uint64						GameStartTime;
	Entity*						PlayerEntity;
	PlayerComponent*			PlayerComponentRef;

	bool						ResetFlag;

	TTF_Font*					GameFont;
	std::unique_ptr<UIManager>	UI;
	UIHealthBar*				HealthBar;
	UIText*						StatusTextUI;

	std::unique_ptr<ObjectPool>	ObjectPoolContext;

	float						SpawnScorpion1Interval;
	float						LastSpawn1Time;
	float						SpawnScorpion2Interval;
	float						LastSpawn2Time;

	bool						Win;
	QuadTree					CollisionTree;
	std::vector<Entity*>		CollisionCandidates;

	std::vector<std::unique_ptr<Timer>>	Timers;
	TimerHandle					NextTimerHandle;

	void						SetStatusText(const std::string& _text);
	void						UpdateTimers();
	void						UpdateCamera();

public:
								World(SDL_Renderer* renderer, GameContext& _context);
								~World();

	float						GetElapsedTime();
	void						AddObject(std::unique_ptr<Entity> _entity);
	TimerHandle					ScheduleTimer(float _delay, std::function<void()> _callback);
	void						CancelTimer(TimerHandle _handle);
	void						WinGame();
	void						Reset() { ResetFlag = true; }

	void						Init();
	void						BuildScene();

	void						Start();
	void						Update();
	void						PostUpdate();

	void						Render();
};
