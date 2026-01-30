#pragma once
#include <SDL3/SDL.h>
#include <core/Entity.h>
#include <core/engine/GameServiceHost.h>
#include <core/QuadTree.h>
#include <core/Sprite.h>
#include <core/UI/UIManager.h>
#include <memory>

class GameMode;

class World
{
private:
	void						HandleQueue();

private:
	GameServiceHost&			Services;
	GameMode*					Mode;
	Sprite						Background;
	std::unique_ptr<UIManager>	UI;

	std::vector<Entity::Ptr>	Entities;
	std::vector<Entity::Ptr>	AddQueue;

	Entity*						CameraTarget;
	QuadTree					CollisionTree;
	std::vector<Entity*>		CollisionCandidates;
	void						UpdateCamera();

public:
								World(GameServiceHost& _services);
								~World();

	void						SetGameMode(GameMode* _mode);
	void						AddObject(std::unique_ptr<Entity> _entity);
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
