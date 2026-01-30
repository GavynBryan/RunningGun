#pragma once
#include <core/Component.h>

class AnimationStateMachine;
class PhysicsComponent;

class PatrolAIComponent :
	public Component
{
private:
	int Lives;
	//Timestamp
	float LastTurnAround = 0; 

	float Interval = 1.5f;
	float MoveSpeed;
	AnimationStateMachine* Animator;
	PhysicsComponent* PhysicsHandle;
public:
	PatrolAIComponent(Entity& _entity, GameServiceHost& _context, float _speed);
	~PatrolAIComponent();

	void Start();
	void Update();
	void PostUpdate();

	void ChangeDirection();
	void OnCollide(Entity& _other);

	void Damage();
	void Die();
};
