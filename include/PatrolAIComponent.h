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
public:
	PatrolAIComponent(Entity& _entity, GameContext& _context, float _speed);
	~PatrolAIComponent();

	void Start();
	void Update();
	void PostUpdate();

	void ChangeDirection();
	void OnCollide(Entity& _other);

	void Damage();
	void Die();
};
