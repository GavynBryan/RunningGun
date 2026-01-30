#include <game/components/PlayerComponent.h>
#include <game/components/PhysicsComponent.h>
#include <game/components/ProjectileComponent.h>
#include <game/actions/PlayerActions.h>
#include <core/Entity.h>
#include <core/InputManager.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/InputService.h>
#include <core/engine/RunnerService.h>

PlayerComponent::PlayerComponent(Entity& _entity, GameServiceHost& _context, const PlayerInputConfig& _inputConfig)
	:Component(_entity, _context),
	Lives(5),
	PlayerSpeed(350),
	Animator(nullptr),
	Bullets(_context),
	BulletOffset(32, 18),
	LastShotTime(0),
	IsInvulnerable(false),
	InvulnerabilityEndTime(0),
	IsInputEnabled(true),
	MoveLeftActionHandle(std::make_unique<MoveLeftAction>()),
	MoveRightActionHandle(std::make_unique<MoveRightAction>()),
	JumpActionHandle(std::make_unique<JumpAction>()),
	ShootActionHandle(std::make_unique<ShootAction>()),
	InputConfig(_inputConfig)
{
	//set initial direction (right)
	ParentEntity.SetDirection(1, 0);
	SetupBullets();
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

	HandleAnimations();
}

void PlayerComponent::PostUpdate()
{
	OrientDirection();
}

void PlayerComponent::HandleAnimations()
{
	ParentEntity.GetSprite().SetFlipX(ParentEntity.GetDirection().x < 0);

	if (ParentEntity.GetVelocity().x != 0) {
		Animator->PlayAnimation("walk");
	} else {
		Animator->PlayAnimation("idle");
	}

}
//load up the bullets into the object pool
void PlayerComponent::SetupBullets()
{
	for (int _index = 0; _index < 20; _index++) {
		auto _projectile = std::make_unique<Entity>(Context, "sprites/bullet.png", 16, 16);
		std::unique_ptr<ProjectileComponent> _projectileComponent(new ProjectileComponent(*_projectile, Context, 400.0f, ParentEntity));

		_projectile->AttachComponent(std::move(_projectileComponent));
		_projectile->SetTag(bullet);
		Bullets.FeedObject(std::move(_projectile));
	}
}

void PlayerComponent::ShootBullet()
{
	//update the shooting cooldown
	auto _currentTime = Context.Get<RunnerService>().GetElapsedTime();
	if (_currentTime - BulletCoolDown > LastShotTime) {
		LastShotTime = _currentTime;
		//borrow bullet from object pool
		auto _bullet = Bullets.BorrowObject();

		//set position based off of player's direction
		auto _position = ParentEntity.GetPosition();
		float _xOrigin = _position.x +(ParentEntity.GetBoundingRect().width / 2);
		_position.x = _xOrigin + (ParentEntity.GetDirection().x * BulletOffset.x);
		_position.y += BulletOffset.y;
		if (_bullet != nullptr) {
			_bullet->SetPosition(_position);
		}
		if (ParentEntity.GetVelocity().x == 0) {
			ParentEntity.GetSprite().SetFlipX(ParentEntity.GetDirection().x < 0);
			Animator->PlayAnimation("shoot");
		}
	}
}

void PlayerComponent::OrientDirection()
{
	//the direction would just be a normalized version of the last non-zero x velocity
	if (ParentEntity.GetVelocity().x != 0) {
		Vec2 _direction = ParentEntity.GetVelocity();
		_direction.y = 0;
		ParentEntity.SetDirection(VectorMath::Normalize(_direction));
	}
}

void PlayerComponent::SetHorizontalVelocity(float x)
{
	ParentEntity.SetVelocity(x, ParentEntity.GetVelocity().y);
}

void PlayerComponent::SetVerticalVelocity(float y)
{
	ParentEntity.SetVelocity(ParentEntity.GetVelocity().x, y);
}

void PlayerComponent::HandleInput()
{
	auto& _input = Context.Get<InputService>().GetInput();

	SetHorizontalVelocity(0);

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
	ParentEntity.SetVelocity(0, ParentEntity.GetVelocity().y);
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
