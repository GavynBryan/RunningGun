#include <game/components/PhysicsComponent.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/PhysicsService.h>
#include <core/engine/RunnerService.h>
#include <algorithm>
#include <iostream>

PhysicsComponent::PhysicsComponent(Entity& _entity, GameServiceHost& _context)
	:Component(_entity, _context),
	PhysContext(_context.Get<PhysicsService>())
{
	
}


PhysicsComponent::~PhysicsComponent()
{
}

void PhysicsComponent::Update()
{
	float _gravity = PhysContext.GetGravity().y;
	float _currentVelocity = ParentEntity.GetVelocity().y;
	if (_currentVelocity < _gravity) {
		float _result = _currentVelocity + _gravity * (Context.Get<RunnerService>().GetDeltaTime());
		_result = std::min(_result, _gravity);
		ParentEntity.SetVelocity(ParentEntity.GetVelocity().x, _result);
	}
	
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

void PhysicsComponent::OnCollide(Entity& _other)
{

}
