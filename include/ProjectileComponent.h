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
	bool		IsMarkedForDeath = false;

	//flag to prevent the bullet from being destroyed on the same frame it's created on
	//THIS IS AN AWFUL WAY OF DOING THIS
	//But I'm on a 9-day time constraint and I already screwed up by having a bad collision loop, so yay spaghetti
	bool		IsBrandNew = true;
public:
	ProjectileComponent(Entity& _entity, float _speed, Entity& _shooter);
	~ProjectileComponent();

	void Start();
	void Update();
	void PostUpdate();

	void OnCollide(Entity& _other);
};

