#include <core/components/RigidBody2D.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/collision/CollisionService.h>
#include <core/collision/PhysicsService.h>
#include <core/timing/TimeService.h>
#include <core/world/WorldService.h>
#include <algorithm>
#include <cmath>

RigidBody2DComponent::RigidBody2DComponent(Actor& entity, GameServiceHost& context, const Vec2& size)
	: ActorComponent(entity, context)
	, Collisions(context.Get<CollisionService>())
	, Physics(context.Get<PhysicsService>())
	, Time(context.Get<TimeService>())
	, World(context.Get<WorldService>())
	, Size(size)
	, Offset(0.0f, 0.0f)
{
	// Create proxy in CollisionService
	Vec2 pos = entity.GetPosition();
	ProxyOptions options;
	options.Bounds = Rectf(pos.x + Offset.x, pos.y + Offset.y, Size.x, Size.y);
	options.Enabled = true;
	options.IsTrigger = false;
	options.LayerMask = CollisionLayer::Default;
	options.CollisionMask = CollisionLayer::All;
	options.UserData = this;
	ProxyId = Collisions.CreateProxy(options);
	
	// Register with PhysicsService for collision response lookups
	Physics.RegisterRigidBody(ProxyId, this);
}

RigidBody2DComponent::~RigidBody2DComponent()
{
	Physics.UnregisterRigidBody(ProxyId);
	Collisions.DestroyProxy(ProxyId);
}

void RigidBody2DComponent::Start()
{
	UpdateProxyBounds();
}

void RigidBody2DComponent::Update()
{
	const float deltaTime = Time.GetDeltaTime();
	ApplyPhysics(deltaTime);
	ClampToWorldBounds();
	UpdateProxyBounds();
}

void RigidBody2DComponent::SetSize(const Vec2& size)
{
	Size = size;
	UpdateProxyBounds();
}

void RigidBody2DComponent::SetOffset(const Vec2& offset)
{
	Offset = offset;
	UpdateProxyBounds();
}

Rectf RigidBody2DComponent::GetBounds() const
{
	if (auto* proxy = Collisions.GetProxy(ProxyId)) {
		return proxy->Bounds;
	}
	return Rectf();
}

void RigidBody2DComponent::SetEnabled(bool enabled)
{
	if (auto* proxy = Collisions.GetProxy(ProxyId)) {
		proxy->Enabled = enabled;
	}
}

bool RigidBody2DComponent::IsEnabled() const
{
	if (auto* proxy = Collisions.GetProxy(ProxyId)) {
		return proxy->Enabled;
	}
	return false;
}

void RigidBody2DComponent::SetLayerMask(uint32_t mask)
{
	if (auto* proxy = Collisions.GetProxy(ProxyId)) {
		proxy->LayerMask = mask;
	}
}

uint32_t RigidBody2DComponent::GetLayerMask() const
{
	if (auto* proxy = Collisions.GetProxy(ProxyId)) {
		return proxy->LayerMask;
	}
	return CollisionLayer::None;
}

void RigidBody2DComponent::SetCollisionMask(uint32_t mask)
{
	if (auto* proxy = Collisions.GetProxy(ProxyId)) {
		proxy->CollisionMask = mask;
	}
}

uint32_t RigidBody2DComponent::GetCollisionMask() const
{
	if (auto* proxy = Collisions.GetProxy(ProxyId)) {
		return proxy->CollisionMask;
	}
	return CollisionLayer::None;
}

bool RigidBody2DComponent::IsGrounded() const
{
	return std::abs(Owner.GetPosition().y - GroundLevel) <= 0.3f;
}

void RigidBody2DComponent::UpdateProxyBounds()
{
	if (auto* proxy = Collisions.GetProxy(ProxyId)) {
		Vec2 pos = Owner.GetPosition();
		proxy->Bounds = Rectf(pos.x + Offset.x, pos.y + Offset.y, Size.x, Size.y);
	}
}

void RigidBody2DComponent::ApplyPhysics(float deltaTime)
{
	if (deltaTime <= 0.0f) return;

	// Apply accumulated acceleration
	if (AccumulatedAcceleration.x != 0.0f || AccumulatedAcceleration.y != 0.0f) {
		Velocity += (AccumulatedAcceleration * deltaTime);
		AccumulatedAcceleration = Vec2(0.0f, 0.0f);
	}

	// Apply gravity
	const float gravityAccel = Gravity * GravityScale;
	if (gravityAccel > 0.0f && Velocity.y < TerminalVelocity) {
		float result = Velocity.y + gravityAccel * deltaTime;
		Velocity.y = std::min(result, TerminalVelocity);
	}

	// Update entity position
	auto position = Owner.GetPosition();
	position += (Velocity * deltaTime);
	Owner.SetPosition(position);
}

void RigidBody2DComponent::ClampToWorldBounds()
{
	auto position = Owner.GetPosition();
	const auto& bounds = World.GetWorldBounds();

	// Clamp to world bounds horizontally
	position.x = std::max(position.x, bounds.x);
	position.x = std::min(position.x, bounds.x + bounds.width - Size.x);
	
	// Clamp to ground level
	position.y = std::min(position.y, GroundLevel);
	
	Owner.SetPosition(position);
}
