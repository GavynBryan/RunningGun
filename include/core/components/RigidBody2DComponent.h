#pragma once

#include <core/entity/Component.h>
#include <core/collision/QuadTreeProxy.h>
#include <core/math/Rect.h>
#include <core/math/Vec2.h>
#include <core/events/MulticastDelegate.h>

class GameServiceHost;
class CollisionService;
class PhysicsService;
class TimeService;
class WorldService;

// Core physics component representing a 2D rectangular collision body with kinematics.
// Creates a proxy in CollisionService and registers with PhysicsService.
// Handles velocity, acceleration, and gravity simulation.
class RigidBody2DComponent : public ActorComponent
{
public:
	using CollisionEvent = MulticastDelegate<RigidBody2DComponent&, RigidBody2DComponent&>;

	RigidBody2DComponent(Actor& entity, GameServiceHost& context, const Vec2& size);
	~RigidBody2DComponent();

	const char* GetName() const override { return "RigidBody2DComponent"; }

	// Non-copyable
	RigidBody2DComponent(const RigidBody2DComponent&) = delete;
	RigidBody2DComponent& operator=(const RigidBody2DComponent&) = delete;

	// Component lifecycle
	void Start() override;
	void Update() override;

	// Bounds configuration
	void SetSize(const Vec2& size);
	Vec2 GetSize() const { return Size; }
	void SetOffset(const Vec2& offset);
	Vec2 GetOffset() const { return Offset; }
	Rectf GetBounds() const;

	// Enable/disable collision detection
	void SetEnabled(bool enabled);
	bool IsEnabled() const;

	// Collision filtering
	void SetLayerMask(uint32_t mask);
	uint32_t GetLayerMask() const;
	void SetCollisionMask(uint32_t mask);
	uint32_t GetCollisionMask() const;

	// Proxy handle (for collision lookups)
	ProxyHandle GetProxyHandle() const { return ProxyId; }

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
	void SetGravity(float gravity) { Gravity = gravity; }
	float GetGravity() const { return Gravity; }
	void SetGravityScale(float scale) { GravityScale = scale; }
	float GetGravityScale() const { return GravityScale; }
	void SetTerminalVelocity(float velocity) { TerminalVelocity = velocity; }
	float GetTerminalVelocity() const { return TerminalVelocity; }

	// Ground check
	void SetGroundLevel(float level) { GroundLevel = level; }
	float GetGroundLevel() const { return GroundLevel; }
	bool IsGrounded() const;

	// Collision events - dispatched by CollisionResponseSystem
	CollisionEvent OnCollisionEnter;
	CollisionEvent OnCollisionStay;
	CollisionEvent OnCollisionExit;

	// Trigger events - dispatched when one or both bodies are triggers
	CollisionEvent OnTriggerEnter;
	CollisionEvent OnTriggerStay;
	CollisionEvent OnTriggerExit;

private:
	void UpdateProxyBounds();
	void ApplyPhysics(float deltaTime);
	void ClampToWorldBounds();

	CollisionService& Collisions;
	PhysicsService& Physics;
	TimeService& Time;
	WorldService& World;
	ProxyHandle ProxyId = InvalidProxyHandle;
	Vec2 Size;
	Vec2 Offset;

	// Kinematics
	Vec2 Velocity = Vec2(0.0f, 0.0f);
	Vec2 AccumulatedAcceleration = Vec2(0.0f, 0.0f);
	float Gravity = 980.0f;       // Default gravity in pixels/s^2
	float GravityScale = 1.0f;
	float TerminalVelocity = 800.0f;
	float GroundLevel = 550.0f;   // Default ground level
};
