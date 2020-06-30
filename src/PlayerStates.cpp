#include <PlayerStates.h>
#include <PlayerComponent.h>


PlayerState::PlayerState(PlayerComponent& _player)
	:mPlayer(_player){}
PlayerState::~PlayerState(){}
void PlayerState::enterState() {}
void PlayerState::update(){}
void PlayerState::postUpdate(){}
void PlayerState::exitState() {}
void PlayerState::damage() {}

// -- DEFAULT STATE --

defaultPlayerState::defaultPlayerState(PlayerComponent& _player)
	:PlayerState(_player){}
defaultPlayerState::~defaultPlayerState() {}

void defaultPlayerState::enterState() 
{
	//enter default animation
}

void defaultPlayerState::update()
{
	mPlayer.handleInput();
}

void defaultPlayerState::postUpdate()
{

}

void defaultPlayerState::exitState()
{

}

void defaultPlayerState::damage()
{
	mPlayer.switchState("DamageState");
}

// -- Damaged State --

damagePlayerState::damagePlayerState(PlayerComponent& _player)
	:PlayerState(_player) {}
damagePlayerState::~damagePlayerState() {}

void damagePlayerState::enterState()
{
	timestamp = sEnvironment::Instance().getElapsedTime();
	mPlayer.damage();
}

void damagePlayerState::update()
{
	mPlayer.handleInput();
	if (sEnvironment::Instance().getElapsedTime() >= stateLength + timestamp) {
		mPlayer.switchState("DefaultState");
	}
}

// -- Dead State --

deadPlayerState::deadPlayerState(PlayerComponent& _player)
	:PlayerState(_player) {}
deadPlayerState::~deadPlayerState() {}

void deadPlayerState::enterState()
{
	timestamp = sEnvironment::Instance().getElapsedTime();
	mPlayer.freeze();
	sEnvironment::Instance().Reset();
}

void deadPlayerState::update()
{
	if (sEnvironment::Instance().getElapsedTime() >= stateLength) {
		//-tell the game to start over
	}
}

// --Victory State--

victoryPlayerState::victoryPlayerState(PlayerComponent& _player)
	:PlayerState(_player) {}
victoryPlayerState::~victoryPlayerState() {}

void victoryPlayerState::enterState()
{
	mPlayer.freeze();
	//tell listeners
}