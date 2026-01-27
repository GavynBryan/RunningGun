#include <BullComponent.h>
#include <ProjectileComponent.h>



BullComponent::BullComponent(Entity& _entity)
	:Component(_entity),
	Offset1(0,32),
	Offset2(0,55),
	ProjectileOffset(Offset1),
	Lives(45)
{
	std::unique_ptr<BullDefaultState> _defaultState(new BullDefaultState(*this));

	AddState("DefaultState", std::move(_defaultState));

	SetupProjectiles();
	ParentEntity.SetDirection(-1, 0);

}


BullComponent::~BullComponent()
{
}

void BullComponent::Start()
{
	SwitchState("DefaultState");
	Animator = ParentEntity.GetAnimator();
}

void BullComponent::Update()
{
	CurrentState->Update(); 
	Animator->PlayAnimation("default");
}

void BullComponent::PostUpdate()
{
}

void BullComponent::SetupProjectiles()
{
		for (int _index = 0; _index < 10; _index++) {
			std::unique_ptr<Entity>_projectile(new Entity("sprites/waves.png", 32, 32));
			std::unique_ptr<ProjectileComponent> _projectileComponent(new ProjectileComponent(*_projectile, 400.0f, ParentEntity));

		_projectile->AttachComponent(std::move(_projectileComponent));
		_projectile->SetTag(enemy_bullet);
		Projectiles.FeedObject(std::move(_projectile));
	}
}

void BullComponent::Shoot()
{
	auto _projectile = Projectiles.BorrowObject();
	if (_projectile != nullptr) {
		_projectile->SetPosition(ParentEntity.GetPosition() + ProjectileOffset);
		SwitchShootPositions();
		Animator->PlayAnimation("shoot");
	}
}

void BullComponent::SwitchShootPositions() {
	if (ProjectileOffset == Offset1) ProjectileOffset = Offset2;
	else ProjectileOffset = Offset1;
}

void BullComponent::AddState(const std::string& _id, BullStatePtr _state)
{
	States.insert(std::make_pair(_id, std::move(_state)));
}

void BullComponent::SwitchState(const std::string& _id)
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

void BullComponent::OnCollide(Entity& _other)
{
	if (_other.GetTag() == bullet) {
		Damage();
	}
}

void BullComponent::Damage() {
	Animator->PlayAnimation("damage");
	Lives--;
	if (Lives <= 0) {
		Environment::Instance().Win();
		Animator->PlayAnimation("die");
		ParentEntity.Disable();
	}
}
