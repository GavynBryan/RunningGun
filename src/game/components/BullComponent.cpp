#include <game/components/BullComponent.h>
#include <game/components/ProjectileComponent.h>
#include <core/components/AnimatorComponent.h>
#include <core/entity/Entity.h>
#include <core/framework/GameServiceHost.h>
#include <core/world/ObjectPoolService.h>
#include <core/timing/TimeService.h>
#include <core/services/component/ComponentRegistry.h>
#include <memory>

//=============================================================================
// Property Registration
//=============================================================================
BEGIN_PROPERTIES(BullComponent)
	PROPERTY(int, Lives, 45,
		.DisplayName = "Lives",
		.Tooltip = "Boss health / hits to kill",
		.Min = 1,
		.Max = 999,
	)
END_PROPERTIES(BullComponent)

//=============================================================================
// Constructor
//=============================================================================
BullComponent::BullComponent(Actor& _entity, GameServiceHost& _context)
	: ActorComponent(_entity, _context)
	, ObjectPool(_context.Get<ObjectPoolService>())
	, Time(_context.Get<TimeService>())
	, Animator(nullptr)
	, CurrentState(nullptr)
	, Offset1(0,32)
	, Offset2(0,55)
	, ProjectileOffset(Offset1)
	, m_Lives(45)
{
	std::unique_ptr<BullDefaultState> _defaultState(new BullDefaultState(*this));

	AddState("DefaultState", std::move(_defaultState));

	GetOwner()->SetDirection(-1, 0);
}


BullComponent::~BullComponent()
{
}

void BullComponent::Start()
{
	SwitchState("DefaultState");
	Animator = GetOwner()->GetComponent<AnimatorComponent>();
}

void BullComponent::Update()
{
	CurrentState->Update(); 
	Animator->PlayAnimation("default");
}

void BullComponent::Shoot()
{
	Actor* owner = GetOwner();
	auto* _projectile = ObjectPool.FetchPrefab("waves");
	if (_projectile != nullptr) {
		if (auto* _projectileComponent = _projectile->GetComponent<ProjectileComponent>()) {
			_projectileComponent->Activate(owner);
		}
		_projectile->SetPosition(owner->GetPosition() + ProjectileOffset);
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
	m_Lives--;
	if (m_Lives <= 0) {
		// Broadcast through component's own delegate
		OnDied.Broadcast(GetOwner());
		Animator->PlayAnimation("die");
		GetOwner()->Disable();
	}
}
