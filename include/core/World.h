#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include "Entity.h"

class ObjectPool;
class PlayerComponent;

class World
{
private:
	typedef std::pair<Entity*, Entity*> CollisionPair;
private:
	void						handleQueue();

private:
	SDL_Renderer*				mRenderer;
	Sprite						mBackground;

	std::vector<Entity::Ptr>	mEntities;
	std::vector<Entity::Ptr>	addQueue;

	Uint64						gameStartTime;
	Entity*						mPlayer;
	PlayerComponent*			mPlayerComponent;

	bool						resetFlag;

	TTF_Font*					gameFont;
	SDL_Texture*				statusTexture;
	SDL_FRect					statusRect;
	std::string					statusText;
	std::vector<Entity::Ptr>	mHearts;

	std::unique_ptr<ObjectPool>	mObjectPool;

	float						spawnScorpion1Interval;
	float						lastSpawn1Time;
	float						spawnScorpion2Interval;
	float						lastSpawn2Time;

	bool						win;

	void						updateStatusText(const std::string& text);

public:
								World(SDL_Renderer* renderer);
								~World();

	float						getElapsedTime();
	void						addObject(std::unique_ptr<Entity> _entity);
	void						winGame();
	void						reset() { resetFlag = true; }

	void						init();
	void						buildScene();

	void						handleCollisions(CollisionPair pairs);

	void						start();
	void						update();
	void						postUpdate();

	void						render();
	void						drawHearts(int i);
};
