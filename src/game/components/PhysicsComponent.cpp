#include <game/components/PhysicsComponent.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/collision/PhysicsService.h>
#include <core/timing/TimeService.h>
#include <algorithm>
#include <cmath>

PhysicsComponent::PhysicsComponent(Actor& entity, GameServiceHost& context, const Vec2& size)
	: ActorComponent(entity, context)
	, Time(context.Get<TimeService>())
	, Physics(context.Get<PhysicsService>())
	, Body(std::make_unique<RigidBody2D>(&entity, size))
	, Velocity(0.0f, 0.0f)
	, AccumulatedAcceleration(0.0f, 0.0f)
	, GravityScale(1.0f)
{
	Physics.Register(Body.get());
}

PhysicsComponent::~PhysicsComponent()
{
	Physics.Unregister(Body.get());
}

void PhysicsComponent::Start()
{
	// Sync initial position
	Body->SyncPosition(Owner.GetPosition());
}

void PhysicsComponent::Update()
{
	const float deltaTime = Time.GetDeltaTime();

	// Apply accumulated acceleration
	if (AccumulatedAcceleration.x != 0.0f || AccumulatedAcceleration.y != 0.0f) {
		Velocity += (AccumulatedAcceleration * deltaTime);
		AccumulatedAcceleration = Vec2(0.0f, 0.0f);
	}

	// Apply gravity
	const float gravityAccel = Physics.GetGravity().y * GravityScale;
	const float terminalVelocity = Physics.GetTerminalVelocity();
	if (gravityAccel > 0.0f && Velocity.y < terminalVelocity) {
		float result = Velocity.y + gravityAccel * deltaTime;
		Velocity.y = std::min(result, terminalVelocity);
	}

	// Update entity position
	auto position = Owner.GetPosition();
	position += (Velocity * deltaTime);
	Owner.SetPosition(position);

	// Sync rigidbody position
	Body->SyncPosition(position);

	// Clamp to world bounds
	const auto& bounds = Physics.GetWorldBounds();
	position.x = std::max(position.x, bounds.x);
	position.x = std::min(position.x, bounds.x + bounds.width - Body->GetSize().x);
	position.y = std::min(position.y, Physics.GetGroundLevel());
	
	Owner.SetPosition(position);
	Body->SyncPosition(position);
}

bool PhysicsComponent::IsGrounded() const
{
	return (std::abs(Owner.GetPosition().y - Physics.GetGroundLevel())) <= 0.3f;
}
