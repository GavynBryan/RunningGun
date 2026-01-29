#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Entity.h"
#include "QuadTree.h"
#include "Timer.h"
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

	std::vector<Entity::Ptr>	Entities;
	std::vector<Entity::Ptr>	AddQueue;

	Uint64						GameStartTime;
	Entity*						PlayerEntity;
	PlayerComponent*			PlayerComponentRef;

	bool						ResetFlag;

	TTF_Font*					GameFont;
	SDL_Texture*				StatusTexture;
	SDL_FRect					StatusRect;
	std::string					StatusText;
	std::vector<Entity::Ptr>	Hearts;

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

	void						UpdateStatusText(const std::string& _text);
	void						UpdateTimers();

public:
								World(SDL_Renderer* renderer);
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
	void						DrawHearts(int _heartCount);
};
