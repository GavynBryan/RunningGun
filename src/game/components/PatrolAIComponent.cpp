#include <game/components/PatrolAIComponent.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/timing/TimeService.h>
#include <core/components/AnimatorComponent.h>
#include <core/components/RigidBody2D.h>

PatrolAIComponent::PatrolAIComponent(Actor& _entity, GameServiceHost& _context, float _speed)
	: ActorComponent(_entity, _context)
	, Time(_context.Get<TimeService>())
	, MoveSpeed(_speed)
	, PhysicsHandle(nullptr)
{
}


PatrolAIComponent::~PatrolAIComponent()
{
}

void PatrolAIComponent::Start()
{
	Lives = 2;
	Animator = Owner.GetComponent<AnimatorComponent>();
	LastTurnAround = Time.GetElapsedTime();
	PhysicsHandle = Owner.GetComponent<RigidBody2DComponent>();
}

void PatrolAIComponent::Update()
{
	if (PhysicsHandle && PhysicsHandle->IsGrounded()) {
		Vec2 _patrolVelocity = Owner.GetDirection();
		_patrolVelocity.y = PhysicsHandle->GetVelocity().y;
		_patrolVelocity.x *= MoveSpeed;
		PhysicsHandle->SetVelocity(_patrolVelocity);
		float _currentTime = Time.GetElapsedTime();

		if (_currentTime - Interval > LastTurnAround) {
			ChangeDirection();
			LastTurnAround = _currentTime;
		}
	}
	else if (PhysicsHandle) {
		PhysicsHandle->SetVelocity(0.0f, PhysicsHandle->GetVelocity().y);
	}

	Owner.GetSprite().SetFlipX(Owner.GetDirection().x < 0);
	Animator->PlayAnimation("idle");
}

void PatrolAIComponent::ChangeDirection()
{
	Owner.SetDirection(Owner.GetDirection() * -1.0f);
}

void PatrolAIComponent::OnCollide(Actor& _other)
{
	if (_other.GetTag() == bullet) {
		Damage();
	}
}

void PatrolAIComponent::Damage()
{
	Lives--;
	if (Lives <= 0) {
		Owner.Disable();
		return;
	}
	Owner.GetSprite().SetFlipX(Owner.GetDirection().x < 0);
	Animator->PlayAnimation("damage");
}

//absolutely useless
void PatrolAIComponent::Die()
{
	Owner.Disable();
}
