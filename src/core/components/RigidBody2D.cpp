#include <core/components/RigidBody2D.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/collision/CollisionService.h>
#include <core/collision/PhysicsService.h>
#include <core/timing/TimeService.h>
#include <core/world/WorldService.h>
#include <core/services/component/ComponentFactory.h>
#include <algorithm>
#include <cmath>

//=============================================================================
// Property Registration
//=============================================================================
BEGIN_PROPERTIES(RigidBody2DComponent)
	PROPERTY(float, Gravity, 980.0f,
		.DisplayName = "Gravity",
		.Tooltip = "Gravity acceleration in pixels/sec²",
		.Min = 0.0f,
		.Max = 5000.0f,
	)
	PROPERTY(float, GravityScale, 1.0f,
		.DisplayName = "Gravity Scale",
		.Tooltip = "Multiplier for gravity (0 = no gravity)",
		.Min = 0.0f,
		.Max = 10.0f,
	)
	PROPERTY(float, TerminalVelocity, 800.0f,
		.DisplayName = "Terminal Velocity",
		.Tooltip = "Maximum fall speed in pixels/sec",
		.Min = 0.0f,
		.Max = 2000.0f,
	)
	PROPERTY(float, GroundLevel, 550.0f,
		.DisplayName = "Ground Level",
		.Tooltip = "Y position of the ground plane",
		.Min = 0.0f,
		.Max = 2000.0f,
	)
	PROPERTY(Vec2, Size, Vec2(64.0f, 64.0f),
		.DisplayName = "Collision Size",
		.Tooltip = "Width and height of collision bounds",
	)
END_PROPERTIES(RigidBody2DComponent)

//=============================================================================
// Constructors
//=============================================================================
RigidBody2DComponent::RigidBody2DComponent(Actor& entity, GameServiceHost& context)
	: ActorComponent(entity, context)
	, Collisions(context.Get<CollisionService>())
	, Physics(context.Get<PhysicsService>())
	, Time(context.Get<TimeService>())
	, World(context.Get<WorldService>())
	, Offset(0.0f, 0.0f)
	, m_Gravity(980.0f)
	, m_GravityScale(1.0f)
	, m_TerminalVelocity(800.0f)
	, m_GroundLevel(550.0f)
	, m_Size(entity.GetWidth(), entity.GetHeight())
{
	// Proxy will be created in Start() after deserialization
}

RigidBody2DComponent::RigidBody2DComponent(Actor& entity, GameServiceHost& context, const Vec2& size)
	: ActorComponent(entity, context)
	, Collisions(context.Get<CollisionService>())
	, Physics(context.Get<PhysicsService>())
	, Time(context.Get<TimeService>())
	, World(context.Get<WorldService>())
	, Offset(0.0f, 0.0f)
	, m_Gravity(980.0f)
	, m_GravityScale(1.0f)
	, m_TerminalVelocity(800.0f)
	, m_GroundLevel(550.0f)
	, m_Size(size)
{
	InitializeProxy(size);
}

RigidBody2DComponent::~RigidBody2DComponent()
{
	if (ProxyId != InvalidProxyHandle) {
		Physics.UnregisterRigidBody(ProxyId);
		Collisions.DestroyProxy(ProxyId);
	}
}

void RigidBody2DComponent::InitializeProxy(const Vec2& size)
{
	if (ProxyId != InvalidProxyHandle) return; // Already initialized
	
	Actor* owner = GetOwner();
	Vec2 pos = owner->GetPosition();
	ProxyOptions options;
	options.Bounds = Rectf(pos.x + Offset.x, pos.y + Offset.y, size.x, size.y);
	options.Enabled = true;
	options.IsTrigger = false;
	options.LayerMask = CollisionLayer::Default;
	options.CollisionMask = CollisionLayer::All;
	options.UserData = this;
	ProxyId = Collisions.CreateProxy(options);
	
	Physics.RegisterRigidBody(ProxyId, this);
}

void RigidBody2DComponent::Start()
{
	// Initialize proxy if not already done (serialization path)
	InitializeProxy(m_Size);
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
	m_Size = size;
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
	return std::abs(GetOwner()->GetPosition().y - m_GroundLevel) <= 0.3f;
}

void RigidBody2DComponent::UpdateProxyBounds()
{
	if (auto* proxy = Collisions.GetProxy(ProxyId)) {
		Vec2 pos = GetOwner()->GetPosition();
		proxy->Bounds = Rectf(pos.x + Offset.x, pos.y + Offset.y, m_Size.x, m_Size.y);
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
	const float gravityAccel = m_Gravity * m_GravityScale;
	if (gravityAccel > 0.0f && Velocity.y < m_TerminalVelocity) {
		float result = Velocity.y + gravityAccel * deltaTime;
		Velocity.y = std::min(result, m_TerminalVelocity);
	}

	// Update entity position
	Actor* owner = GetOwner();
	auto position = owner->GetPosition();
	position += (Velocity * deltaTime);
	owner->SetPosition(position);
}

void RigidBody2DComponent::ClampToWorldBounds()
{
	Actor* owner = GetOwner();
	auto position = owner->GetPosition();
	const auto& bounds = World.GetWorldBounds();

	// Clamp to world bounds horizontally
	position.x = std::max(position.x, bounds.x);
	position.x = std::min(position.x, bounds.x + bounds.width - m_Size.x);
	
	// Clamp to ground level
	position.y = std::min(position.y, m_GroundLevel);
	
	owner->SetPosition(position);
}
