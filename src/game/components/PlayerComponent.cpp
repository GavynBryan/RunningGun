#include <game/components/PlayerComponent.h>
#include <core/components/RigidBody2D.h>
#include <core/components/AnimatorComponent.h>
#include <game/components/ProjectileComponent.h>
#include <game/actions/PlayerActions.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/input/InputService.h>
#include <core/world/ObjectPoolService.h>
#include <core/timing/TimeService.h>
#include <core/math/MathUtils.h>
#include <core/services/component/ComponentRegistry.h>
#include <game/input/PlayerInputConfigService.h>

//=============================================================================
// Property Registration
//=============================================================================
BEGIN_PROPERTIES(PlayerComponent)
	PROPERTY(int, Lives, 5,
		.DisplayName = "Starting Lives",
		.Tooltip = "Number of lives the player starts with",
		.Min = 1,
		.Max = 99,
	)
	PROPERTY(float, PlayerSpeed, 350.0f,
		.DisplayName = "Player Speed",
		.Tooltip = "Maximum horizontal movement speed in pixels/sec",
		.Min = 0.0f,
		.Max = 1000.0f,
	)
	PROPERTY(float, GroundAcceleration, 2000.0f,
		.DisplayName = "Ground Acceleration",
		.Tooltip = "How quickly the player accelerates on ground (pixels/sec²)",
		.Min = 0.0f,
		.Max = 10000.0f,
	)
	PROPERTY(float, GroundDeceleration, 3500.0f,
		.DisplayName = "Ground Deceleration",
		.Tooltip = "How quickly the player decelerates when not moving (pixels/sec²)",
		.Min = 0.0f,
		.Max = 10000.0f,
	)
END_PROPERTIES(PlayerComponent)

//=============================================================================
// Constructor
//=============================================================================
PlayerComponent::PlayerComponent(Actor& _entity, GameServiceHost& _context)
	: ActorComponent(_entity, _context)
	, Time(_context.Get<TimeService>())
	, Input(_context.Get<InputService>())
	, ObjectPool(_context.Get<ObjectPoolService>())
	, InputConfig(_context.Get<PlayerInputConfigService>().GetConfig())
	, Animator(nullptr)
	, PhysicsHandle(nullptr)
	, MoveLeftActionHandle(std::make_unique<MoveLeftAction>())
	, MoveRightActionHandle(std::make_unique<MoveRightAction>())
	, JumpActionHandle(std::make_unique<JumpAction>())
	, ShootActionHandle(std::make_unique<ShootAction>())
	, BulletOffset(32, 18)
	, MovementIntent(0.0f, 0.0f)
	, m_Lives(5)
	, m_PlayerSpeed(350.0f)
	, m_GroundAcceleration(2000.0f)
	, m_GroundDeceleration(3500.0f)
	, LastShotTime(0)
	, IsInvulnerable(false)
	, InvulnerabilityEndTime(0)
	, IsInputEnabled(true)
{
	//set initial direction (right)
	Actor* owner = GetOwner();
	owner->SetDirection(1, 0);
}


PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Start()
{
	LastShotTime = 0;
	IsInvulnerable = false;
	IsInputEnabled = true;

	Actor* owner = GetOwner();
	Animator = owner->GetComponent<AnimatorComponent>();
	PhysicsHandle = owner->GetComponent<RigidBody2DComponent>();
}

void PlayerComponent::Update()
{
	// Check invulnerability cooldown
	if (IsInvulnerable) {
		if (Time.GetElapsedTime() >= InvulnerabilityEndTime) {
			IsInvulnerable = false;
		}
	}

	if (IsInputEnabled) {
		HandleInput();
	}

	ApplyMovementIntent();
	HandleAnimations();
	OrientDirection();
}

void PlayerComponent::HandleAnimations()
{
	Actor* owner = GetOwner();
	owner->SetFlipX(owner->GetDirection().x < 0);
	Vec2 _velocity = PhysicsHandle ? PhysicsHandle->GetVelocity() : Vec2(0.0f, 0.0f);

	if (_velocity.x != 0) {
		Animator->PlayAnimation("walk");
	} else {
		Animator->PlayAnimation("idle");
	}

}

void PlayerComponent::ShootBullet()
{
	//update the shooting cooldown
	auto _currentTime = Time.GetElapsedTime();
	if (_currentTime - BulletCoolDown > LastShotTime) {
		LastShotTime = _currentTime;
		//borrow bullet from object pool
		auto* _bullet = ObjectPool.FetchPrefab("bullet");

		Actor* owner = GetOwner();
		//set position based off of player's direction
		auto _position = owner->GetPosition();
		float _xOrigin = _position.x + (owner->GetWidth() / 2);
		_position.x = _xOrigin + (owner->GetDirection().x * BulletOffset.x);
		_position.y += BulletOffset.y;
		if (_bullet != nullptr) {
			if (auto* _projectileComponent = _bullet->GetComponent<ProjectileComponent>()) {
				_projectileComponent->Activate(owner);
			}
			_bullet->SetPosition(_position);
		}
		if (!PhysicsHandle || PhysicsHandle->GetVelocity().x == 0) {
			owner->SetFlipX(owner->GetDirection().x < 0);
			Animator->PlayAnimation("shoot");
		}
	}
}

void PlayerComponent::OrientDirection()
{
	//the direction would just be a normalized version of the last non-zero x velocity
	if (PhysicsHandle && PhysicsHandle->GetVelocity().x != 0) {
		Vec2 _direction = PhysicsHandle->GetVelocity();
		_direction.y = 0;
		GetOwner()->SetDirection(VectorMath::Normalize(_direction));
	}
}

bool PlayerComponent::IsGrounded() const
{
	return PhysicsHandle && PhysicsHandle->IsGrounded();
}

void PlayerComponent::SetMovementIntentX(float x)
{
	MovementIntent.x = x;
}

void PlayerComponent::RequestJump(float jumpSpeed)
{
	if (PhysicsHandle && PhysicsHandle->IsGrounded()) {
		SetVerticalVelocity(-jumpSpeed);
	}
}

void PlayerComponent::ApplyMovementIntent()
{
	if (!PhysicsHandle) {
		return;
	}
	const float _deltaTime = Time.GetDeltaTime();
	if (_deltaTime <= 0.0f) {
		return;
	}
	const float _targetVelocity = MovementIntent.x * m_PlayerSpeed;
	const float _currentVelocity = PhysicsHandle->GetVelocityX();
	const float _accel = (std::abs(_targetVelocity) > std::abs(_currentVelocity))
		? m_GroundAcceleration
		: m_GroundDeceleration;
	const float _newVelocity = MathUtils::MoveToward(_currentVelocity, _targetVelocity, _accel * _deltaTime);
	PhysicsHandle->SetVelocityX(_newVelocity);
	MovementIntent.x = 0.0f;
}

void PlayerComponent::SetHorizontalVelocity(float x)
{
	if (!PhysicsHandle) {
		return;
	}
	PhysicsHandle->SetVelocityX(x);
}

void PlayerComponent::SetVerticalVelocity(float y)
{
	if (!PhysicsHandle) {
		return;
	}
	PhysicsHandle->SetVelocityY(y);
}

void PlayerComponent::HandleInput()
{
	auto& _input = Input.GetInput();

	MovementIntent = Vec2(0.0f, 0.0f);

	// Movement - use held state for continuous movement
	if (_input.IsKeyHeld(InputConfig.GetBinding(InputAction::MoveLeft))) {
		MoveLeftActionHandle->Execute(*this);
	}
	if (_input.IsKeyHeld(InputConfig.GetBinding(InputAction::MoveRight))) {
		MoveRightActionHandle->Execute(*this);
	}

	// Jump - use pressed state to only jump on initial press
	if (_input.IsKeyPressed(InputConfig.GetBinding(InputAction::Jump))) {
		JumpActionHandle->Execute(*this);
	}

	// Fire - use pressed state to only fire on initial press (not held)
	if (_input.IsKeyPressed(InputConfig.GetBinding(InputAction::Shoot))) {
		ShootActionHandle->Execute(*this);
	}

}

void PlayerComponent::Freeze()
{
	MovementIntent.x = 0.0f;
	SetHorizontalVelocity(0.0f);
	if (PhysicsHandle) {
		PhysicsHandle->ClearAcceleration();
	}
}

void PlayerComponent::OnDamage()
{
	if (IsInvulnerable) {
		return;
	}

	m_Lives--;

	// Play damage animation
	Actor* owner = GetOwner();
	owner->SetFlipX(owner->GetDirection().x < 0);
	Animator->PlayAnimation("damage");

	if (m_Lives <= 0) {
		OnDeath();
	} else {
		IsInvulnerable = true;
		InvulnerabilityEndTime = Time.GetElapsedTime() + InvulnerabilityDuration;
	}
}

void PlayerComponent::OnDeath()
{
	IsInputEnabled = false;
	Freeze();

	// Broadcast through component's own delegate
	OnDied.Broadcast(GetOwner());
}

void PlayerComponent::OnVictory()
{
	IsInputEnabled = false;
	Freeze();
}

void PlayerComponent::OnCollide(Actor& _other)
{
	if (_other.GetTag() == hazard || _other.GetTag() == enemy_bullet) {
		OnDamage();
	}
}
