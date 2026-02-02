#pragma once
#include <core/base/ActorComponent.h>
#include <core/base/ComponentMacros.h>

class AnimatorComponent;
class RigidBody2DComponent;
class TimeService;

class PatrolAIComponent :
	public ActorComponent
{
	COMPONENT(PatrolAIComponent, "patrol_ai")

public:
	const char* GetName() const override { return "PatrolAIComponent"; }

private:
	TimeService& Time;
	AnimatorComponent* Animator;
	RigidBody2DComponent* PhysicsHandle;

	// Serialized properties
	float m_MoveSpeed;
	float m_Interval;
	int m_StartingLives;

	// Runtime state
	int Lives;
	float LastTurnAround = 0;

public:
	PatrolAIComponent(Actor& _entity, GameServiceHost& _context);
	~PatrolAIComponent();

	void Start();
	void Update();

	void ChangeDirection();
	void OnCollide(Actor& _other);

	void Damage();
	void Die();
};
