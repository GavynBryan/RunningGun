#include <PlayerComponent.h>
#include <PhysicsComponent.h>
#include <ProjectileComponent.h>
#include <utility.h>
#include <core/Entity.h>
#include <core/InputManager.h>

PlayerComponent::PlayerComponent(Entity& _entity)
	:Component(_entity),
	PlayerSpeed(350),
	Lives(5),
	BulletOffset(32, 18),
	Animator(nullptr),
	CurrentState(nullptr),
	LastShotTime(0)
{
	//initialize states
	std::unique_ptr<DefaultPlayerState>		_defaultState(new DefaultPlayerState(*this));
	std::unique_ptr<DamagePlayerState>		_damageState(new DamagePlayerState(*this));
	std::unique_ptr<DeadPlayerState>		_deadState(new DeadPlayerState(*this));
	std::unique_ptr<VictoryPlayerState>		_victoryState(new VictoryPlayerState(*this));

	AddState("DefaultState", std::move(_defaultState));
	AddState("DamageState", std::move(_damageState));
	AddState("DeadState", std::move(_deadState));
	AddState("VictoryState", std::move(_victoryState));

	//set initial direction (right)
	ParentEntity.SetDirection(1, 0);
	SetupBullets();
}


PlayerComponent::~PlayerComponent()
{
}

void PlayerComponent::Start()
{
	SwitchState("DefaultState");
	LastShotTime = 0;

	Animator = ParentEntity.GetAnimator();
}

void PlayerComponent::Update()
{
	CurrentState->Update();
	HandleAnimations();
}

void PlayerComponent::PostUpdate()
{
	CurrentState->PostUpdate();
	OrientDirection();
}

void PlayerComponent::HandleAnimations()
{
	if (ParentEntity.GetDirection().x > 0) {
		if (ParentEntity.GetVelocity().x != 0)
			Animator->PlayAnimation("walkright");
		else Animator->PlayAnimation("right");
	} else{
		if (ParentEntity.GetVelocity().x != 0)
			Animator->PlayAnimation("walkleft");
		else Animator->PlayAnimation("left");
	}

}
//load up the bullets into the object pool
void PlayerComponent::SetupBullets()
{
	for (int _index = 0; _index < 20; _index++) {
		std::unique_ptr<Entity>_projectile(new Entity("sprites/bullet.png", 16, 16));
		std::unique_ptr<ProjectileComponent> _projectileComponent(new ProjectileComponent(*_projectile, 400.0f, ParentEntity));

		_projectile->AttachComponent(std::move(_projectileComponent));
		_projectile->SetTag(bullet);
		Bullets.FeedObject(std::move(_projectile));
	}
}

void PlayerComponent::ShootBullet()
{
	//update the shooting cooldown
	auto _currentTime = Environment::Instance().GetElapsedTime();
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
			if(ParentEntity.GetDirection().x > 0)
				Animator->PlayAnimation("shootright");
			else
				Animator->PlayAnimation("shootleft");
		}
	}
}

void PlayerComponent::AddState(const std::string& _id, StatePtr _state)
{
	States.insert(std::make_pair(_id, std::move(_state)));
}

void PlayerComponent::SwitchState(const std::string& _id)
{
	auto _nextState = States.find(_id);
	//if it can't access the state, we need to close it
	assert(_nextState != States.end());
	if (_nextState->second.get() == CurrentState) { return; }
	if (CurrentState != nullptr) {
		CurrentState->ExitState();
	}
	CurrentState = _nextState->second.get();
	CurrentState->EnterState();
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

void PlayerComponent::HandleInput()
{
	auto& _input = InputManager::Instance();

	Vec2 _velocity = Vec2(0, ParentEntity.GetVelocity().y);

	// Movement - use held state for continuous movement
	if (_input.IsKeyHeld(SDL_SCANCODE_LEFT)) {
		_velocity = Vec2(-PlayerSpeed, ParentEntity.GetVelocity().y);
	}
	if (_input.IsKeyHeld(SDL_SCANCODE_RIGHT)) {
		_velocity = Vec2(PlayerSpeed, ParentEntity.GetVelocity().y);
	}

	// Jump - use pressed state to only jump on initial press
	if (_input.IsKeyPressed(SDL_SCANCODE_Z)) {
		if (ParentEntity.IsGrounded()) {
			_velocity = Vec2(ParentEntity.GetVelocity().x, -500);
		}
	}

	// Fire - use pressed state to only fire on initial press (not held)
	if (_input.IsKeyPressed(SDL_SCANCODE_X)) {
		ShootBullet();
	}

	ParentEntity.SetVelocity(_velocity);
}

void PlayerComponent::Freeze()
{
	ParentEntity.SetVelocity(0, ParentEntity.GetVelocity().y);
}

void PlayerComponent::OnCollide(Entity& _other)
{
	if (_other.GetTag() == hazard || _other.GetTag() == enemy_bullet) {
		CurrentState->Damage();
	}
}

void PlayerComponent::Damage()
{
	Lives--;
	if (ParentEntity.GetDirection().x > 0)
		Animator->PlayAnimation("damageright");
	else
		Animator->PlayAnimation("damageleft");
	if (Lives <= 0) {
		SwitchState("DeadState");
	}
}
