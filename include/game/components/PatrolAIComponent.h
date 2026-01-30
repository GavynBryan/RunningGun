#pragma once
#include <memory>
#include <string_view>
#include <core/Component.h>
#include <core/PropertyDescriptor.h>

class AnimationStateMachine;
class PhysicsComponent;

class PatrolAIComponent :
	public Component
{
public:
	// ========== Serialization ==========
	static constexpr const char* TypeName = "patrol_ai";
	static constexpr PropertyDescriptor Properties[] = {
		{"speed", "float", "150.0", "Movement speed"},
	};
	static std::unique_ptr<Component> Create(Entity& entity, GameServiceHost& context, std::string_view paramsJson);

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
