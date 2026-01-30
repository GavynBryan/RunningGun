#pragma once
#include <memory>
#include <string_view>
#include <core/Component.h>
#include <core/PropertyDescriptor.h>

class PhysicsComponent;

class ProjectileComponent
	:public Component
{
public:
	// ========== Serialization ==========
	static constexpr const char* TypeName = "projectile";
	static constexpr PropertyDescriptor Properties[] = {
		{"speed", "float", "400.0", "Movement speed"},
		{"lifeSpan", "float", "3.0", "Seconds before destruction"},
	};
	static std::unique_ptr<Component> Create(Entity& entity, GameServiceHost& context, std::string_view paramsJson);

private:
	float		Speed;
	float		SpawnTime = 0;
	float		LifeSpan = 3.0;

	Entity*	Shooter = nullptr;
	PhysicsComponent* PhysicsHandle = nullptr;
public:
	ProjectileComponent(Entity& _entity, GameServiceHost& _context, float _speed, float _lifeSpan = 3.0f);
	~ProjectileComponent();

	void Start();
	void Update();
	void PostUpdate();

	void Activate(Entity* _shooter);
	void SetShooter(Entity* _shooter);

	void OnCollide(Entity& _other);
};
