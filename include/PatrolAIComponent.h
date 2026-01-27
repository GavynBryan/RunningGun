#pragma once
#include <core/Component.h>

class AnimationListener;

class PatrolAIComponent :
	public Component
{
private:
	int Lives;
	//Timestamp
	float LastTurnAround = 0; 

	float Interval = 1.5f;
	float MoveSpeed;
	AnimationListener* Animator;
	bool MarkedForDeath = false;
public:
	PatrolAIComponent(Entity& _entity, float _speed);
	~PatrolAIComponent();

	void Start();
	void Update();
	void PostUpdate();

	void ChangeDirection();
	void OnCollide(Entity& _other);

	void Damage();
	void Die();
};

