#include <game/components/ProjectileComponent.h>
#include <core/framework/GameServiceHost.h>
#include <core/timing/TimeService.h>
#include <core/components/RigidBody2D.h>



ProjectileComponent::ProjectileComponent(Actor& _entity, GameServiceHost& _context, float _speed, float _lifeSpan)
	: ActorComponent(_entity, _context)
	, Time(_context.Get<TimeService>())
	, Speed(_speed)
	, LifeSpan(_lifeSpan)
{
}


ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Start()
{
	SpawnTime = Time.GetElapsedTime();
	PhysicsHandle = Owner.GetComponent<RigidBody2DComponent>();
	if (Shooter) {
		Owner.SetDirection(Shooter->GetDirection());
	}
}

void ProjectileComponent::Update()
{
	if (Time.GetElapsedTime() > SpawnTime + LifeSpan){
		Owner.Disable();
		return;
	}
	if (PhysicsHandle) {
		PhysicsHandle->SetVelocity((Owner.GetDirection() * Speed));
	}
}

void ProjectileComponent::Activate(Actor* _shooter)
{
	SetShooter(_shooter);
	SpawnTime = Time.GetElapsedTime();
}

void ProjectileComponent::SetShooter(Actor* _shooter)
{
	Shooter = _shooter;
	if (Shooter) {
		Owner.SetDirection(Shooter->GetDirection());
	}
}

void ProjectileComponent::OnCollide(Actor& _other)
{
	//don't let scorpions block the bull from getting the player
	if (Owner.GetTag() == enemy_bullet && _other.GetTag() == hazard) return;
	//don't detect collsion with its own shooter or other projectiles
	if ((!Shooter || &_other != Shooter) && _other.GetTag() != bullet && _other.GetTag() != enemy_bullet) {
		Owner.Disable();
	}
}
