#include <game/components/BullComponent.h>
#include <game/components/ProjectileComponent.h>
#include <core/Entity.h>
#include <core/engine/GameServiceHost.h>
#include <core/engine/ObjectPoolService.h>
#include <memory>



BullComponent::BullComponent(Entity& _entity, GameServiceHost& _context)
	:Component(_entity, _context),
	Offset1(0,32),
	Offset2(0,55),
	ProjectileOffset(Offset1),
	Lives(45)
{
	std::unique_ptr<BullDefaultState> _defaultState(new BullDefaultState(*this));

	AddState("DefaultState", std::move(_defaultState));

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

void BullComponent::Shoot()
{
	auto* _projectile = Context.Get<ObjectPoolService>().FetchPrefab("waves");
	if (_projectile != nullptr) {
		if (auto* _projectileComponent = _projectile->GetComponent<ProjectileComponent>()) {
			_projectileComponent->Activate(&ParentEntity);
		}
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
		// Broadcast through component's own delegate
		OnDied.Broadcast(&ParentEntity);
		Animator->PlayAnimation("die");
		ParentEntity.Disable();
	}
}
