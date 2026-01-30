#include <iostream>
#include <game/components/ProjectileComponent.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/RunnerService.h>



ProjectileComponent::ProjectileComponent(Entity& _entity, GameServiceHost& _context, float _speed, float _lifeSpan)
	:Component(_entity, _context),
	Speed(_speed),
	LifeSpan(_lifeSpan)
{
}


ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Start()
{
	SpawnTime = Context.Get<RunnerService>().GetElapsedTime();
	if (Shooter) {
		ParentEntity.SetDirection(Shooter->GetDirection());
	}
}

void ProjectileComponent::Update()
{
	if (Context.Get<RunnerService>().GetElapsedTime() > SpawnTime + LifeSpan){
		ParentEntity.Disable();
		return;
	}
	ParentEntity.SetVelocity((ParentEntity.GetDirection() * Speed));
}

void ProjectileComponent::PostUpdate()
{
}

void ProjectileComponent::Activate(Entity* _shooter)
{
	SetShooter(_shooter);
	SpawnTime = Context.Get<RunnerService>().GetElapsedTime();
}

void ProjectileComponent::SetShooter(Entity* _shooter)
{
	Shooter = _shooter;
	if (Shooter) {
		ParentEntity.SetDirection(Shooter->GetDirection());
	}
}

void ProjectileComponent::OnCollide(Entity& _other)
{
	//don't let scorpions block the bull from getting the player
	if (ParentEntity.GetTag() == enemy_bullet && _other.GetTag() == hazard) return;
	//don't detect collsion with its own shooter or other projectiles
	if ((!Shooter || &_other != Shooter) && _other.GetTag() != bullet && _other.GetTag() != enemy_bullet) {
		ParentEntity.Disable();
	}
}
