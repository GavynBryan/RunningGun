#pragma once
#include <core/Component.h>

class ProjectileComponent
	:public Component
{
private:
	float		Speed;
	float		SpawnTime = 0;
	float		LifeSpan = 3.0;

	Entity&	Shooter;
public:
	ProjectileComponent(Entity& _entity, GameServiceHost& _context, float _speed, Entity& _shooter);
	~ProjectileComponent();

	void Start();
	void Update();
	void PostUpdate();

	void OnCollide(Entity& _other);
};
