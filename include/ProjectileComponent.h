#pragma once
#include <core/Component.h>

class ProjectileComponent
	:public Component
{
private:
	float		mSpeed;
	float		spawnTime = 0;
	float		lifeSpan = 3.0;

	Entity&	mShooter;
	bool		isMarkedForDeath = false;

	//flag to prevent the bullet from being destroyed on the same frame it's created on
	//THIS IS AN AWFUL WAY OF DOING THIS
	//But I'm on a 9-day time constraint and I already screwed up by having a bad collision loop, so yay spaghetti
	bool		isBrandNew = true;
public:
	ProjectileComponent(Entity& _entity, float _speed, Entity& _shooter);
	~ProjectileComponent();

	void start();
	void update();
	void postUpdate();

	void onCollide(Entity& _other);
};

