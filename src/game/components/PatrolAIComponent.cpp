#include <game/components/PatrolAIComponent.h>
#include <core/Entity.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/RunnerService.h>
#include <core/animation/AnimationStateMachine.h>

PatrolAIComponent::PatrolAIComponent(Entity& _entity, GameServiceHost& _context, float _speed)
	:Component(_entity, _context),
	MoveSpeed(_speed)
{
}


PatrolAIComponent::~PatrolAIComponent()
{
}

void PatrolAIComponent::Start()
{
	Lives = 2;
	Animator = ParentEntity.GetAnimator();
	LastTurnAround = Context.Get<RunnerService>().GetElapsedTime();
}

void PatrolAIComponent::Update()
{
	if (ParentEntity.IsGrounded()) {
		Vec2 _patrolVelocity = ParentEntity.GetDirection();
		_patrolVelocity.y = ParentEntity.GetVelocity().y;
		_patrolVelocity.x *= MoveSpeed;
		ParentEntity.SetVelocity(_patrolVelocity);
		float _currentTime = Context.Get<RunnerService>().GetElapsedTime();

		if (_currentTime - Interval > LastTurnAround) {
			ChangeDirection();
			LastTurnAround = _currentTime;
		}
	}
	else { ParentEntity.SetVelocity(0, ParentEntity.GetVelocity().y); }
}

void PatrolAIComponent::PostUpdate()
{
	ParentEntity.GetSprite().SetFlipX(ParentEntity.GetDirection().x < 0);
	Animator->PlayAnimation("idle");
}

void PatrolAIComponent::ChangeDirection()
{
	ParentEntity.SetDirection(ParentEntity.GetDirection() * -1.0f);
}

void PatrolAIComponent::OnCollide(Entity& _other)
{
	if (_other.GetTag() == bullet) {
		Damage();
	}
}

void PatrolAIComponent::Damage()
{
	Lives--;
	if (Lives <= 0) {
		ParentEntity.Disable();
		return;
	}
	ParentEntity.GetSprite().SetFlipX(ParentEntity.GetDirection().x < 0);
	Animator->PlayAnimation("damage");
}

//absolutely useless
void PatrolAIComponent::Die()
{
	ParentEntity.Disable();
}
