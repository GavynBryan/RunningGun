#include <game/components/PlayerComponent.h>
#include <game/components/PhysicsComponent.h>
#include <game/components/ProjectileComponent.h>
#include <game/actions/PlayerActions.h>
#include <core/Entity.h>
#include <core/Json.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/InputService.h>
#include <core/engine/ObjectPoolService.h>
#include <core/engine/RunnerService.h>
#include <core/MathUtils.h>

std::unique_ptr<Component> PlayerComponent::Create(Entity& entity, GameServiceHost& context, std::string_view paramsJson)
{
	float groundAcceleration = 2000.0f;
	float groundDeceleration = 3500.0f;
	if (!paramsJson.empty()) {
		auto result = Json::Parse(std::string(paramsJson));
		if (!result.error()) {
			groundAcceleration = static_cast<float>(Json::GetDouble(result.value(), "groundAcceleration", groundAcceleration));
			groundDeceleration = static_cast<float>(Json::GetDouble(result.value(), "groundDeceleration", groundDeceleration));
		}
	}
	auto component = std::make_unique<PlayerComponent>(entity, context);
	component->SetGroundAcceleration(groundAcceleration);
	component->SetGroundDeceleration(groundDeceleration);
	return component;
}

PlayerComponent::PlayerComponent(Entity& _entity, GameServiceHost& _context)
	:Component(_entity, _context),
	Lives(5),
	PlayerSpeed(350),
	GroundAcceleration(2000.0f),
	GroundDeceleration(3500.0f),
	Animator(nullptr),
	PhysicsHandle(nullptr),
	BulletOffset(32, 18),
	LastShotTime(0),
	MovementIntent(0.0f, 0.0f),
	IsInvulnerable(false),
	InvulnerabilityEndTime(0),
	IsInputEnabled(true),
	MoveLeftActionHandle(std::make_unique<MoveLeftAction>()),
	MoveRightActionHandle(std::make_unique<MoveRightAction>()),
	JumpActionHandle(std::make_unique<JumpAction>()),
	ShootActionHandle(std::make_unique<ShootAction>())
{
	//set initial direction (right)
	ParentEntity.SetDirection(1, 0);
}


PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Start()
{
	LastShotTime = 0;
	IsInvulnerable = false;
	IsInputEnabled = true;

	Animator = ParentEntity.GetAnimator();
	PhysicsHandle = ParentEntity.GetComponent<PhysicsComponent>();
}

void PlayerComponent::Update()
{
	// Check invulnerability cooldown
	if (IsInvulnerable) {
		if (Context.Get<RunnerService>().GetElapsedTime() >= InvulnerabilityEndTime) {
			IsInvulnerable = false;
		}
	}

	if (IsInputEnabled) {
		HandleInput();
	}

	ApplyMovementIntent();
	HandleAnimations();
}

void PlayerComponent::PostUpdate()
{
	OrientDirection();
}

void PlayerComponent::HandleAnimations()
{
	ParentEntity.GetSprite().SetFlipX(ParentEntity.GetDirection().x < 0);
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
	auto _currentTime = Context.Get<RunnerService>().GetElapsedTime();
	if (_currentTime - BulletCoolDown > LastShotTime) {
		LastShotTime = _currentTime;
		//borrow bullet from object pool
		auto* _bullet = Context.Get<ObjectPoolService>().FetchPrefab("bullet");

		//set position based off of player's direction
		auto _position = ParentEntity.GetPosition();
		float _xOrigin = _position.x +(ParentEntity.GetBoundingRect().width / 2);
		_position.x = _xOrigin + (ParentEntity.GetDirection().x * BulletOffset.x);
		_position.y += BulletOffset.y;
		if (_bullet != nullptr) {
			if (auto* _projectileComponent = _bullet->GetComponent<ProjectileComponent>()) {
				_projectileComponent->Activate(&ParentEntity);
			}
			_bullet->SetPosition(_position);
		}
		if (!PhysicsHandle || PhysicsHandle->GetVelocity().x == 0) {
			ParentEntity.GetSprite().SetFlipX(ParentEntity.GetDirection().x < 0);
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
		ParentEntity.SetDirection(VectorMath::Normalize(_direction));
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
	const float _deltaTime = Context.Get<RunnerService>().GetDeltaTime();
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
	auto& inputService = Context.Get<InputService>();
	auto& _input = inputService.GetInput();
	const auto& inputConfig = inputService.GetPlayerInputConfig();

	MovementIntent = Vec2(0.0f, 0.0f);

	// Movement - use held state for continuous movement
	if (_input.IsKeyHeld(inputConfig.GetBinding(InputAction::MoveLeft))) {
		MoveLeftActionHandle->Execute(*this);
	}
	if (_input.IsKeyHeld(inputConfig.GetBinding(InputAction::MoveRight))) {
		MoveRightActionHandle->Execute(*this);
	}

	// Jump - use pressed state to only jump on initial press
	if (_input.IsKeyPressed(inputConfig.GetBinding(InputAction::Jump))) {
		JumpActionHandle->Execute(*this);
	}

	// Fire - use pressed state to only fire on initial press (not held)
	if (_input.IsKeyPressed(inputConfig.GetBinding(InputAction::Shoot))) {
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
	ParentEntity.GetSprite().SetFlipX(ParentEntity.GetDirection().x < 0);
	Animator->PlayAnimation("damage");

	if (Lives <= 0) {
		OnDeath();
	} else {
		IsInvulnerable = true;
		InvulnerabilityEndTime = Context.Get<RunnerService>().GetElapsedTime() + InvulnerabilityDuration;
	}
}

void PlayerComponent::OnDeath()
{
	IsInputEnabled = false;
	Freeze();

	// Broadcast through component's own delegate
	OnDied.Broadcast(&ParentEntity);
}

void PlayerComponent::OnVictory()
{
	IsInputEnabled = false;
	Freeze();
}

void PlayerComponent::OnCollide(Entity& _other)
{
	if (_other.GetTag() == hazard || _other.GetTag() == enemy_bullet) {
		OnDamage();
	}
}
