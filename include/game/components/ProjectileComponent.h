#pragma once
#include <core/Component.h>

class ProjectileComponent
	:public Component
{
private:
	float		Speed;
	float		SpawnTime = 0;
	float		LifeSpan = 3.0;

	Entity*	Shooter = nullptr;
public:
	ProjectileComponent(Entity& _entity, GameServiceHost& _context, float _speed, float _lifeSpan = 3.0f);
	~ProjectileComponent();

	void Start();
	void Update();
	void PostUpdate();

	void Activate(Entity* _shooter);
	void SetShooter(Entity* _shooter);

	void OnCollide(Entity& _other);
};
