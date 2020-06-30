#include <iostream>
#include <ProjectileComponent.h>
#include <core/sEnvironment.h>



ProjectileComponent::ProjectileComponent(Entity& _entity, float _speed, Entity& _shooter)
	:Component(_entity),
	mSpeed(_speed),
	mShooter(_shooter)
{
}


ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::start()
{
	isBrandNew = true;
	isMarkedForDeath = false;
	spawnTime = sEnvironment::Instance().getElapsedTime();
	mEntity.setDirection(mShooter.getDirection());
}

void ProjectileComponent::update()
{
	if (sEnvironment::Instance().getElapsedTime() > spawnTime + lifeSpan){
		mEntity.disable();
		return;
	}
	mEntity.setVelocity((mEntity.getDirection() * mSpeed));
}

void ProjectileComponent::postUpdate()
{
	if (isMarkedForDeath) {
		mEntity.disable();
	}
	isBrandNew = false;
}

void ProjectileComponent::onCollide(Entity& _other)
{
	if (isBrandNew) return;
	//don't let scorpions block the bull from getting the player
	if (mEntity.getTag() == enemy_bullet && _other.getTag() == hazard) return;
	//don't detect collsion with its own shooter or other projectiles
	if (&_other != &mShooter && _other.getTag() != bullet && _other.getTag() != enemy_bullet) {
		isMarkedForDeath = true;
	}
}