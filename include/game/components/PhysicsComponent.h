#pragma once
#include <memory>
#include <string_view>
#include <core/Component.h>
#include <core/PropertyDescriptor.h>
#include <core/Vec2.h>

class GameServiceHost;
class PhysicsService;

class PhysicsComponent :
	public Component
{
public:
	// ========== Serialization ==========
	static constexpr const char* TypeName = "physics";
	static constexpr PropertyDescriptor Properties[] = {
		{"gravityScale", "float", "1.0", "Multiplier for gravity effect"},
	};
	static std::unique_ptr<Component> Create(Entity& entity, GameServiceHost& context, std::string_view paramsJson);

private:
	PhysicsService& PhysContext;
	Vec2 Velocity;
	Vec2 AccumulatedAcceleration;
	float GravityScale;
public:
	PhysicsComponent(Entity& _entity, GameServiceHost& _context);
	~PhysicsComponent();

	void Update();
	void PostUpdate();

	Vec2 GetVelocity() const;
	float GetVelocityX() const { return Velocity.x; }
	float GetVelocityY() const { return Velocity.y; }
	void SetVelocity(const Vec2& _velocity);
	void SetVelocity(float _x, float _y);
	void SetVelocityX(float _x);
	void SetVelocityY(float _y);
	void AddVelocity(const Vec2& _delta);
	void AddAcceleration(const Vec2& _acceleration);
	void ClearAcceleration();
	void SetGravityScale(float _scale);
	float GetGravityScale() const { return GravityScale; }
	bool IsGrounded() const;

	void OnCollide(Entity& _other);
};
