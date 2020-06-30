#pragma once
#include <core/Component.h>

class AnimationListener;

class PatrolAIComponent :
	public Component
{
private:
	int lives;
	//Timestamp
	float lastTurnAround = 0; 

	float interval = 1.5f;
	float moveSpeed;
	AnimationListener* mAnimator;
	bool markedForDeath = false;
public:
	PatrolAIComponent(Entity& _entity, float _speed);
	~PatrolAIComponent();

	void start();
	void update();
	void postUpdate();

	void changeDirection();
	void onCollide(Entity& _other);

	void damage();
	void die();
};

