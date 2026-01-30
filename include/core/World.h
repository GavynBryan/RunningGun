#pragma once
#include <SDL3/SDL.h>
#include <core/Entity.h>
#include <core/engine/EngineServices.h>
#include <core/QuadTree.h>
#include <core/Sprite.h>
#include <core/Timer.h>
#include <core/UI/UIManager.h>
#include <memory>

class GameMode;

class World
{
private:
	void						HandleQueue();

private:
	SDL_Renderer*				Renderer;
	EngineServices&				Services;
	GameMode*					Mode;
	Sprite						Background;
	std::unique_ptr<UIManager>	UI;

	std::vector<Entity::Ptr>	Entities;
	std::vector<Entity::Ptr>	AddQueue;

	Uint64						GameStartTime;
	Entity*						CameraTarget;
	QuadTree					CollisionTree;
	std::vector<Entity*>		CollisionCandidates;

	std::vector<std::unique_ptr<Timer>>	Timers;
	TimerHandle					NextTimerHandle;

	void						UpdateTimers();
	void						UpdateCamera();

public:
								World(SDL_Renderer* renderer, EngineServices& _services);
								~World();

	void						SetGameMode(GameMode* _mode);
	float						GetElapsedTime();
	void						ResetElapsedTime();
	void						AddObject(std::unique_ptr<Entity> _entity);
	TimerHandle					ScheduleTimer(float _delay, std::function<void()> _callback);
	void						CancelTimer(TimerHandle _handle);
	void						ClearEntities();
	void						Reset();
	GameMode*					GetGameMode() const { return Mode; }
	UIManager*					GetUI() const { return UI.get(); }
	void						SetBackgroundTexture(SDL_Texture* _texture);
	void						SetCameraTarget(Entity* _entity);

	void						Init();
	void						BuildScene();

	void						Start();
	void						Update();
	void						PostUpdate();

	void						Render();
};
