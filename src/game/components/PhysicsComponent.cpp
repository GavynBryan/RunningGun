#include <game/components/PhysicsComponent.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/PhysicsService.h>
#include <core/engine/RunnerService.h>
#include <algorithm>
#include <cmath>

PhysicsComponent::PhysicsComponent(Entity& _entity, GameServiceHost& _context)
	:Component(_entity, _context),
	PhysContext(_context.Get<PhysicsService>()),
	Velocity(0.0f, 0.0f),
	AccumulatedAcceleration(0.0f, 0.0f),
	GravityScale(1.0f)
{
	
}


PhysicsComponent::~PhysicsComponent()
{
}

void PhysicsComponent::Update()
{
	const float _deltaTime = Context.Get<RunnerService>().GetDeltaTime();
	if (AccumulatedAcceleration.x != 0.0f || AccumulatedAcceleration.y != 0.0f) {
		Velocity += (AccumulatedAcceleration * _deltaTime);
		AccumulatedAcceleration = Vec2(0.0f, 0.0f);
	}

	const float _gravityAccel = PhysContext.GetGravity().y * GravityScale;
	const float _terminalVelocity = PhysContext.GetTerminalVelocity();
	if (_gravityAccel > 0.0f && Velocity.y < _terminalVelocity) {
		float _result = Velocity.y + _gravityAccel * _deltaTime;
		Velocity.y = std::min(_result, _terminalVelocity);
	}

	auto _position = ParentEntity.GetPosition();
	_position += (Velocity * _deltaTime);
	ParentEntity.SetPosition(_position);
}

void PhysicsComponent::PostUpdate()
{
	auto _position = ParentEntity.GetPosition();
	const auto& _bounds = PhysContext.GetWorldBounds();

	_position.x = std::max(_position.x, _bounds.x);
	_position.x = std::min(_position.x, _bounds.x + _bounds.w - ParentEntity.GetWidth());
	_position.y = std::min(_position.y, PhysContext.GetGroundLevel());
	ParentEntity.SetPosition(_position);
}

Vec2 PhysicsComponent::GetVelocity() const
{
	return Velocity;
}

void PhysicsComponent::SetVelocity(const Vec2& _velocity)
{
	Velocity = _velocity;
}

void PhysicsComponent::SetVelocity(float _x, float _y)
{
	Velocity.x = _x;
	Velocity.y = _y;
}

void PhysicsComponent::SetVelocityX(float _x)
{
	Velocity.x = _x;
}

void PhysicsComponent::SetVelocityY(float _y)
{
	Velocity.y = _y;
}

void PhysicsComponent::AddVelocity(const Vec2& _delta)
{
	Velocity += _delta;
}

void PhysicsComponent::AddAcceleration(const Vec2& _acceleration)
{
	AccumulatedAcceleration += _acceleration;
}

void PhysicsComponent::ClearAcceleration()
{
	AccumulatedAcceleration = Vec2(0.0f, 0.0f);
}

void PhysicsComponent::SetGravityScale(float _scale)
{
	GravityScale = _scale;
}

bool PhysicsComponent::IsGrounded() const
{
	return (std::abs(ParentEntity.GetPosition().y - PhysContext.GetGroundLevel())) <= 0.3f;
}

void PhysicsComponent::OnCollide(Entity& _other)
{

}
