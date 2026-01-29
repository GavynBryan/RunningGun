#include <game/components/PatrolAIComponent.h>
#include <core/Entity.h>
#include <core/engine/GameContext.h>
#include <core/animation/AnimationListener.h>

PatrolAIComponent::PatrolAIComponent(Entity& _entity, GameContext& _context, float _speed)
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
	LastTurnAround = Context.GetElapsedTime();
}

void PatrolAIComponent::Update()
{
	if (ParentEntity.IsGrounded()) {
		Vec2 _patrolVelocity = ParentEntity.GetDirection();
		_patrolVelocity.y = ParentEntity.GetVelocity().y;
		_patrolVelocity.x *= MoveSpeed;
		ParentEntity.SetVelocity(_patrolVelocity);
		float _currentTime = Context.GetElapsedTime();

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
