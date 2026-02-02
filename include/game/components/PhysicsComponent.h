#pragma once
#include <core/entity/Component.h>
#include <core/RigidBody2D.h>
#include <core/math/Vec2.h>
#include <memory>

class GameServiceHost;
class PhysicsService;
class TimeService;

class PhysicsComponent : public ActorComponent
{
public:
	PhysicsComponent(Actor& entity, GameServiceHost& context, const Vec2& size);
	~PhysicsComponent();

	const char* GetName() const override { return "PhysicsComponent"; }

	// Non-copyable
	PhysicsComponent(const PhysicsComponent&) = delete;
	PhysicsComponent& operator=(const PhysicsComponent&) = delete;

	void Start() override;
	void Update() override;

	// Velocity
	Vec2 GetVelocity() const { return Velocity; }
	float GetVelocityX() const { return Velocity.x; }
	float GetVelocityY() const { return Velocity.y; }
	void SetVelocity(const Vec2& velocity) { Velocity = velocity; }
	void SetVelocity(float x, float y) { Velocity = Vec2(x, y); }
	void SetVelocityX(float x) { Velocity.x = x; }
	void SetVelocityY(float y) { Velocity.y = y; }
	void AddVelocity(const Vec2& delta) { Velocity += delta; }

	// Acceleration
	void AddAcceleration(const Vec2& acceleration) { AccumulatedAcceleration += acceleration; }
	void ClearAcceleration() { AccumulatedAcceleration = Vec2(0.0f, 0.0f); }

	// Gravity
	void SetGravityScale(float scale) { GravityScale = scale; }
	float GetGravityScale() const { return GravityScale; }

	// Ground check
	bool IsGrounded() const;

	// RigidBody access
	RigidBody2D* GetRigidBody() { return Body.get(); }
	const RigidBody2D* GetRigidBody() const { return Body.get(); }

private:
	TimeService& Time;
	PhysicsService& Physics;
	std::unique_ptr<RigidBody2D> Body;
	Vec2 Velocity;
	Vec2 AccumulatedAcceleration;
	float GravityScale = 1.0f;
};
