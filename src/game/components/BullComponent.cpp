#include <game/components/BullComponent.h>
#include <game/components/ProjectileComponent.h>
#include <core/components/AnimatorComponent.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/world/ObjectPoolService.h>
#include <core/timing/TimeService.h>
#include <memory>



BullComponent::BullComponent(Actor& _entity, GameServiceHost& _context)
	: ActorComponent(_entity, _context)
	, ObjectPool(_context.Get<ObjectPoolService>())
	, Time(_context.Get<TimeService>())
	, Offset1(0,32)
	, Offset2(0,55)
	, ProjectileOffset(Offset1)
	, Lives(45)
{
	std::unique_ptr<BullDefaultState> _defaultState(new BullDefaultState(*this));

	AddState("DefaultState", std::move(_defaultState));

	Owner.SetDirection(-1, 0);

}


BullComponent::~BullComponent()
{
}

void BullComponent::Start()
{
	SwitchState("DefaultState");
	Animator = Owner.GetComponent<AnimatorComponent>();
}

void BullComponent::Update()
{
	CurrentState->Update(); 
	Animator->PlayAnimation("default");
}

void BullComponent::Shoot()
{
	auto* _projectile = ObjectPool.FetchPrefab("waves");
	if (_projectile != nullptr) {
		if (auto* _projectileComponent = _projectile->GetComponent<ProjectileComponent>()) {
			_projectileComponent->Activate(&Owner);
		}
		_projectile->SetPosition(Owner.GetPosition() + ProjectileOffset);
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

void BullComponent::OnCollide(Actor& _other)
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
		OnDied.Broadcast(&Owner);
		Animator->PlayAnimation("die");
		Owner.Disable();
	}
}
