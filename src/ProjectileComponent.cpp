#include <iostream>
#include <ProjectileComponent.h>
#include <core/Environment.h>



ProjectileComponent::ProjectileComponent(Entity& _entity, float _speed, Entity& _shooter)
	:Component(_entity),
	Speed(_speed),
	Shooter(_shooter)
{
}


ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Start()
{
	IsBrandNew = true;
	IsMarkedForDeath = false;
	SpawnTime = Environment::Instance().GetElapsedTime();
	ParentEntity.SetDirection(Shooter.GetDirection());
}

void ProjectileComponent::Update()
{
	if (Environment::Instance().GetElapsedTime() > SpawnTime + LifeSpan){
		ParentEntity.Disable();
		return;
	}
	ParentEntity.SetVelocity((ParentEntity.GetDirection() * Speed));
}

void ProjectileComponent::PostUpdate()
{
	if (IsMarkedForDeath) {
		ParentEntity.Disable();
	}
	IsBrandNew = false;
}

void ProjectileComponent::OnCollide(Entity& _other)
{
	if (IsBrandNew) return;
	//don't let scorpions block the bull from getting the player
	if (ParentEntity.GetTag() == enemy_bullet && _other.GetTag() == hazard) return;
	//don't detect collsion with its own shooter or other projectiles
	if (&_other != &Shooter && _other.GetTag() != bullet && _other.GetTag() != enemy_bullet) {
		IsMarkedForDeath = true;
	}
}