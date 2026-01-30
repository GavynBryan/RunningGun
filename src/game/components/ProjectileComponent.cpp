#include <game/components/ProjectileComponent.h>
#include <core/Json.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/RunnerService.h>
#include <game/components/PhysicsComponent.h>

std::unique_ptr<Component> ProjectileComponent::Create(Entity& entity, GameServiceHost& context, std::string_view paramsJson)
{
	float speed = 400.0f;
	float lifeSpan = 3.0f;
	if (!paramsJson.empty()) {
		auto result = Json::Parse(std::string(paramsJson));
		if (!result.error()) {
			speed = static_cast<float>(Json::GetDouble(result.value(), "speed", speed));
			lifeSpan = static_cast<float>(Json::GetDouble(result.value(), "lifeSpan", lifeSpan));
		}
	}
	return std::make_unique<ProjectileComponent>(entity, context, speed, lifeSpan);
}

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
	PhysicsHandle = ParentEntity.GetComponent<PhysicsComponent>();
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
	if (PhysicsHandle) {
		PhysicsHandle->SetVelocity((ParentEntity.GetDirection() * Speed));
	}
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
