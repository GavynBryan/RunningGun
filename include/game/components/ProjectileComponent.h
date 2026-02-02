#pragma once
#include <core/base/ActorComponent.h>
#include <core/serialization/Serializable.h>

class RigidBody2DComponent;
class TimeService;

class ProjectileComponent
	:public ActorComponent
{
	SERIALIZABLE_COMPONENT(ProjectileComponent, "projectile")

public:
	const char* GetName() const override { return "ProjectileComponent"; }

private:
	TimeService& Time;
	RigidBody2DComponent* PhysicsHandle = nullptr;

	// Serialized properties
	float m_Speed;
	float m_LifeSpan;

	// Runtime state
	float		SpawnTime = 0;
	Actor*		Shooter = nullptr;

public:
	ProjectileComponent(Actor& _entity, GameServiceHost& _context);
	~ProjectileComponent();

	void Start();
	void Update();

	void Activate(Actor* _shooter);
	void SetShooter(Actor* _shooter);

	void OnCollide(Actor& _other);
};
