#include <iostream>
#include <game/components/ProjectileComponent.h>
#include <core/engine/GameplayServices.h>



ProjectileComponent::ProjectileComponent(Entity& _entity, GameplayServices& _context, float _speed, Entity& _shooter)
	:Component(_entity, _context),
	Speed(_speed),
	Shooter(_shooter)
{
}


ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Start()
{
	SpawnTime = Context.GetElapsedTime();
	ParentEntity.SetDirection(Shooter.GetDirection());
}

void ProjectileComponent::Update()
{
	if (Context.GetElapsedTime() > SpawnTime + LifeSpan){
		ParentEntity.Disable();
		return;
	}
	ParentEntity.SetVelocity((ParentEntity.GetDirection() * Speed));
}

void ProjectileComponent::PostUpdate()
{
}

void ProjectileComponent::OnCollide(Entity& _other)
{
	//don't let scorpions block the bull from getting the player
	if (ParentEntity.GetTag() == enemy_bullet && _other.GetTag() == hazard) return;
	//don't detect collsion with its own shooter or other projectiles
	if (&_other != &Shooter && _other.GetTag() != bullet && _other.GetTag() != enemy_bullet) {
		ParentEntity.Disable();
	}
}
