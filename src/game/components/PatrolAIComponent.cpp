#include <game/components/PatrolAIComponent.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/timing/TimeService.h>
#include <core/components/AnimatorComponent.h>
#include <core/components/RigidBody2D.h>
#include <core/services/component/ComponentFactory.h>

//=============================================================================
// Property Registration
//=============================================================================
BEGIN_PROPERTIES(PatrolAIComponent)
	PROPERTY(float, MoveSpeed, 150.0f,
		.DisplayName = "Move Speed",
		.Tooltip = "Patrol movement speed in pixels/sec",
		.Min = 0.0f,
		.Max = 500.0f,
	)
	PROPERTY(float, Interval, 1.5f,
		.DisplayName = "Turn Interval",
		.Tooltip = "Seconds between direction changes",
		.Min = 0.1f,
		.Max = 10.0f,
	)
	PROPERTY(int, StartingLives, 2,
		.DisplayName = "Starting Lives",
		.Tooltip = "Hits before death",
		.Min = 1,
		.Max = 10,
	)
END_PROPERTIES(PatrolAIComponent)

//=============================================================================
// Constructor
//=============================================================================
PatrolAIComponent::PatrolAIComponent(Actor& _entity, GameServiceHost& _context)
	: ActorComponent(_entity, _context)
	, Time(_context.Get<TimeService>())
	, Animator(nullptr)
	, PhysicsHandle(nullptr)
	, m_MoveSpeed(150.0f)
	, m_Interval(1.5f)
	, m_StartingLives(2)
	, Lives(2)
	, LastTurnAround(0)
{
}


PatrolAIComponent::~PatrolAIComponent()
{
}

void PatrolAIComponent::Start()
{
	Lives = m_StartingLives;
	Actor* owner = GetOwner();
	Animator = owner->GetComponent<AnimatorComponent>();
	LastTurnAround = Time.GetElapsedTime();
	PhysicsHandle = owner->GetComponent<RigidBody2DComponent>();
}

void PatrolAIComponent::Update()
{
	Actor* owner = GetOwner();
	if (PhysicsHandle && PhysicsHandle->IsGrounded()) {
		Vec2 _patrolVelocity = owner->GetDirection();
		_patrolVelocity.y = PhysicsHandle->GetVelocity().y;
		_patrolVelocity.x *= m_MoveSpeed;
		PhysicsHandle->SetVelocity(_patrolVelocity);
		float _currentTime = Time.GetElapsedTime();

		if (_currentTime - m_Interval > LastTurnAround) {
			ChangeDirection();
			LastTurnAround = _currentTime;
		}
	}
	else if (PhysicsHandle) {
		PhysicsHandle->SetVelocity(0.0f, PhysicsHandle->GetVelocity().y);
	}

	owner->GetSprite().SetFlipX(owner->GetDirection().x < 0);
	Animator->PlayAnimation("idle");
}

void PatrolAIComponent::ChangeDirection()
{
	Actor* owner = GetOwner();
	owner->SetDirection(owner->GetDirection() * -1.0f);
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
	Actor* owner = GetOwner();
	if (Lives <= 0) {
		owner->Disable();
		return;
	}
	owner->GetSprite().SetFlipX(owner->GetDirection().x < 0);
	Animator->PlayAnimation("damage");
}

//absolutely useless
void PatrolAIComponent::Die()
{
	GetOwner()->Disable();
}
