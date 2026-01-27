#include <PlayerStates.h>
#include <PlayerComponent.h>


PlayerState::PlayerState(PlayerComponent& _player)
	:PlayerRef(_player){}
PlayerState::~PlayerState(){}
void PlayerState::EnterState() {}
void PlayerState::Update(){}
void PlayerState::PostUpdate(){}
void PlayerState::ExitState() {}
void PlayerState::Damage() {}

// -- DEFAULT STATE --

DefaultPlayerState::DefaultPlayerState(PlayerComponent& _player)
	:PlayerState(_player){}
DefaultPlayerState::~DefaultPlayerState() {}

void DefaultPlayerState::EnterState() 
{
	//enter default animation
}

void DefaultPlayerState::Update()
{
	PlayerRef.HandleInput();
}

void DefaultPlayerState::PostUpdate()
{

}

void DefaultPlayerState::ExitState()
{

}

void DefaultPlayerState::Damage()
{
	PlayerRef.SwitchState("DamageState");
}

// -- Damaged State --

DamagePlayerState::DamagePlayerState(PlayerComponent& _player)
	:PlayerState(_player) {}
DamagePlayerState::~DamagePlayerState() {}

void DamagePlayerState::EnterState()
{
	Timestamp = Environment::Instance().GetElapsedTime();
	PlayerRef.Damage();
}

void DamagePlayerState::Update()
{
	PlayerRef.HandleInput();
	if (Environment::Instance().GetElapsedTime() >= StateLength + Timestamp) {
		PlayerRef.SwitchState("DefaultState");
	}
}

// -- Dead State --

DeadPlayerState::DeadPlayerState(PlayerComponent& _player)
	:PlayerState(_player) {}
DeadPlayerState::~DeadPlayerState() {}

void DeadPlayerState::EnterState()
{
	Timestamp = Environment::Instance().GetElapsedTime();
	PlayerRef.Freeze();
	Environment::Instance().Reset();
}

void DeadPlayerState::Update()
{
	if (Environment::Instance().GetElapsedTime() >= StateLength) {
		//-tell the game to start over
	}
}

// --Victory State--

VictoryPlayerState::VictoryPlayerState(PlayerComponent& _player)
	:PlayerState(_player) {}
VictoryPlayerState::~VictoryPlayerState() {}

void VictoryPlayerState::EnterState()
{
	PlayerRef.Freeze();
	//tell listeners
}
