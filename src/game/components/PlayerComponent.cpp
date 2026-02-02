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

PlayerComponent::PlayerComponent(Actor& _entity, GameServiceHost& _context, const PlayerInputConfig& _inputConfig)
	: ActorComponent(_entity, _context)
	, Time(_context.Get<TimeService>())
	, Input(_context.Get<InputService>())
	, ObjectPool(_context.Get<ObjectPoolService>())
	, Lives(5)
	, PlayerSpeed(350)
	, GroundAcceleration(2000.0f)
	, GroundDeceleration(3500.0f)
	, Animator(nullptr)
	, PhysicsHandle(nullptr)
	, BulletOffset(32, 18)
	, LastShotTime(0)
	, MovementIntent(0.0f, 0.0f)
	, IsInvulnerable(false)
	, InvulnerabilityEndTime(0)
	, IsInputEnabled(true)
	, MoveLeftActionHandle(std::make_unique<MoveLeftAction>())
	, MoveRightActionHandle(std::make_unique<MoveRightAction>())
	, JumpActionHandle(std::make_unique<JumpAction>())
	, ShootActionHandle(std::make_unique<ShootAction>())
	, InputConfig(_inputConfig)
{
	//set initial direction (right)
	Owner.SetDirection(1, 0);
}


PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Start()
{
	LastShotTime = 0;
	IsInvulnerable = false;
	IsInputEnabled = true;

	Animator = Owner.GetComponent<AnimatorComponent>();
	PhysicsHandle = Owner.GetComponent<RigidBody2DComponent>();
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
	Owner.SetFlipX(Owner.GetDirection().x < 0);
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

		//set position based off of player's direction
		auto _position = Owner.GetPosition();
		float _xOrigin = _position.x + (Owner.GetWidth() / 2);
		_position.x = _xOrigin + (Owner.GetDirection().x * BulletOffset.x);
		_position.y += BulletOffset.y;
		if (_bullet != nullptr) {
			if (auto* _projectileComponent = _bullet->GetComponent<ProjectileComponent>()) {
				_projectileComponent->Activate(&Owner);
			}
			_bullet->SetPosition(_position);
		}
		if (!PhysicsHandle || PhysicsHandle->GetVelocity().x == 0) {
			Owner.SetFlipX(Owner.GetDirection().x < 0);
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
		Owner.SetDirection(VectorMath::Normalize(_direction));
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
	const float _targetVelocity = MovementIntent.x * PlayerSpeed;
	const float _currentVelocity = PhysicsHandle->GetVelocityX();
	const float _accel = (std::abs(_targetVelocity) > std::abs(_currentVelocity))
		? GroundAcceleration
		: GroundDeceleration;
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

	Lives--;

	// Play damage animation
	Owner.SetFlipX(Owner.GetDirection().x < 0);
	Animator->PlayAnimation("damage");

	if (Lives <= 0) {
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
	OnDied.Broadcast(&Owner);
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
