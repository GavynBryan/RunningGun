#pragma once
#include <core/entity/Component.h>

class AnimatorComponent;
class RigidBody2DComponent;
class TimeService;

class PatrolAIComponent :
	public ActorComponent
{
public:
	const char* GetName() const override { return "PatrolAIComponent"; }

private:
	TimeService& Time;
	int Lives;
	//Timestamp
	float LastTurnAround = 0; 

	float Interval = 1.5f;
	float MoveSpeed;
	AnimatorComponent* Animator;
	RigidBody2DComponent* PhysicsHandle;
public:
	PatrolAIComponent(Actor& _entity, GameServiceHost& _context, float _speed);
	~PatrolAIComponent();

	void Start();
	void Update();

	void ChangeDirection();
	void OnCollide(Actor& _other);

	void Damage();
	void Die();
};
