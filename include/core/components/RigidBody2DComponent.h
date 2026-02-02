#pragma once

#include <core/base/ActorComponent.h>
#include <core/base/ComponentMacros.h>
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
	COMPONENT(RigidBody2DComponent, "rigidbody2d")

public:
	using CollisionEvent = MulticastDelegate<RigidBody2DComponent&, RigidBody2DComponent&>;

	// Default constructor for serialization - uses entity dimensions
	RigidBody2DComponent(Actor& entity, GameServiceHost& context);
	
	// Explicit size constructor for programmatic creation
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
	Vec2 GetSize() const { return m_Size; }
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
	void SetGravity(float gravity) { m_Gravity = gravity; }
	float GetGravity() const { return m_Gravity; }
	void SetGravityScale(float scale) { m_GravityScale = scale; }
	float GetGravityScale() const { return m_GravityScale; }
	void SetTerminalVelocity(float velocity) { m_TerminalVelocity = velocity; }
	float GetTerminalVelocity() const { return m_TerminalVelocity; }

	// Ground check
	void SetGroundLevel(float level) { m_GroundLevel = level; }
	float GetGroundLevel() const { return m_GroundLevel; }
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
	void InitializeProxy(const Vec2& size);
	void UpdateProxyBounds();
	void ApplyPhysics(float deltaTime);
	void ClampToWorldBounds();

	CollisionService& Collisions;
	PhysicsService& Physics;
	TimeService& Time;
	WorldService& World;
	ProxyHandle ProxyId = InvalidProxyHandle;
	Vec2 Offset;

	// Serialized properties (via PROPERTY macros)
	float m_Gravity;
	float m_GravityScale;
	float m_TerminalVelocity;
	float m_GroundLevel;
	Vec2 m_Size;

	// Runtime state (not serialized)
	Vec2 Velocity = Vec2(0.0f, 0.0f);
	Vec2 AccumulatedAcceleration = Vec2(0.0f, 0.0f);
};
