#include <game/components/ProjectileComponent.h>
#include <core/framework/GameServiceHost.h>
#include <core/timing/TimeService.h>
#include <core/components/RigidBody2D.h>
#include <core/entity/Entity.h>
#include <core/services/component/ComponentFactory.h>

//=============================================================================
// Property Registration
//=============================================================================
BEGIN_PROPERTIES(ProjectileComponent)
	PROPERTY(float, Speed, 400.0f,
		.DisplayName = "Speed",
		.Tooltip = "Projectile travel speed in pixels/sec",
		.Min = 0.0f,
		.Max = 2000.0f,
	)
	PROPERTY(float, LifeSpan, 3.0f,
		.DisplayName = "Life Span",
		.Tooltip = "Seconds before projectile auto-destroys",
		.Min = 0.1f,
		.Max = 30.0f,
	)
END_PROPERTIES(ProjectileComponent)

//=============================================================================
// Constructor
//=============================================================================
ProjectileComponent::ProjectileComponent(Actor& _entity, GameServiceHost& _context)
	: ActorComponent(_entity, _context)
	, Time(_context.Get<TimeService>())
	, PhysicsHandle(nullptr)
	, m_Speed(400.0f)
	, m_LifeSpan(3.0f)
	, SpawnTime(0)
	, Shooter(nullptr)
{
}


ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Start()
{
	SpawnTime = Time.GetElapsedTime();
	Actor* owner = GetOwner();
	PhysicsHandle = owner->GetComponent<RigidBody2DComponent>();
	if (Shooter) {
		owner->SetDirection(Shooter->GetDirection());
	}
}

void ProjectileComponent::Update()
{
	Actor* owner = GetOwner();
	if (Time.GetElapsedTime() > SpawnTime + m_LifeSpan){
		owner->Disable();
		return;
	}
	if (PhysicsHandle) {
		PhysicsHandle->SetVelocity((owner->GetDirection() * m_Speed));
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
		GetOwner()->SetDirection(Shooter->GetDirection());
	}
}

void ProjectileComponent::OnCollide(Actor& _other)
{
	Actor* owner = GetOwner();
	//don't let scorpions block the bull from getting the player
	if (owner->GetTag() == enemy_bullet && _other.GetTag() == hazard) return;
	//don't detect collsion with its own shooter or other projectiles
	if ((!Shooter || &_other != Shooter) && _other.GetTag() != bullet && _other.GetTag() != enemy_bullet) {
		owner->Disable();
	}
}
