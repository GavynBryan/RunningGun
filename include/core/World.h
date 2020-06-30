#pragma once
#include <SFML/Graphics.hpp>
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
	sf::RenderWindow&			mWindow;
	sf::View					renderView;
	sf::Sprite					mBackground;

	//sf::Vector2f				worldSize;

	
	std::vector<Entity::Ptr>	mEntities;
	std::vector<Entity::Ptr>	addQueue;

	sf::Clock					gameClock;
	Entity*						mPlayer;
	PlayerComponent*			mPlayerComponent;

	bool						resetFlag;

	sf::Font					gameFont;
	sf::Text					statusText;
	std::vector<Entity::Ptr>	mHearts;

	std::unique_ptr<ObjectPool>	mObjectPool;

	float						spawnScorpion1Interval;
	float						lastSpawn1Time;
	float						spawnScorpion2Interval;
	float						lastSpawn2Time;

	bool						win;

public:
								World(sf::RenderWindow& _window);
								~World();

	float						getElapsedTime();
	void						addObject(std::unique_ptr<Entity> _entity);
	void						winGame() { 
									win = true; 
									mPlayer->disable(); 
									statusText.setString ("You Win!");
									statusText.setOrigin(statusText.getGlobalBounds().width / 2, 
										statusText.getGlobalBounds().height / 2);
									}
	void						reset() { resetFlag = true; }

	void						init();
	void						buildScene();

	void						handleCollisions(CollisionPair pairs);

	void						start(); //post-init function
	void						update();
	void						postUpdate();

	void						render();
	void						drawHearts(int i);
};

