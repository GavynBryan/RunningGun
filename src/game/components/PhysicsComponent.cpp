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

	float _gravity = PhysContext.GetGravity().y * GravityScale;
	if (_gravity > 0.0f && Velocity.y < _gravity) {
		float _result = Velocity.y + _gravity * _deltaTime;
		Velocity.y = std::min(_result, _gravity);
	}

	auto _position = ParentEntity.GetPosition();
	_position += (Velocity * _deltaTime);
	ParentEntity.SetPosition(_position);
}

void PhysicsComponent::PostUpdate()
{
	auto _position = ParentEntity.GetPosition();

	_position.x = std::min(_position.x, 600.0f);
	_position.x = std::max(_position.x, 0.0f);
	//_position.y = std::max(_position.y, 0.0f);
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
