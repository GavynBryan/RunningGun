#pragma once
#include <core/entity/Component.h>

class RigidBody2DComponent;
class TimeService;

class ProjectileComponent
	:public ActorComponent
{
public:
	const char* GetName() const override { return "ProjectileComponent"; }

private:
	TimeService& Time;
	float		Speed;
	float		SpawnTime = 0;
	float		LifeSpan = 3.0;

	Actor*	Shooter = nullptr;
	RigidBody2DComponent* PhysicsHandle = nullptr;
public:
	ProjectileComponent(Actor& _entity, GameServiceHost& _context, float _speed, float _lifeSpan = 3.0f);
	~ProjectileComponent();

	void Start();
	void Update();

	void Activate(Actor* _shooter);
	void SetShooter(Actor* _shooter);

	void OnCollide(Actor& _other);
};
