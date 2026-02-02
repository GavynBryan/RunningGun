#include <BullStates.h>
#include <game/components/BullComponent.h>
#include <core/timing/TimeService.h>



BullState::BullState(BullComponent& _bull)
	:BullRef(_bull)
{
}

BullState::~BullState()
{}

void BullState::EnterState(){}
void BullState::Update(){}
void BullState::ExitState(){}


//Default State
BullDefaultState::BullDefaultState(BullComponent& _bull)
	:BullState(_bull),
	LastShot(0)
{
}
BullDefaultState::~BullDefaultState()
{}

void BullDefaultState::EnterState() 
{
	LastShot = 0;
}

void BullDefaultState::Update() 
{
	float _currentTime = BullRef.GetTime().GetElapsedTime();
	if (_currentTime - LastShot > ShootFrequency) {
		BullRef.Shoot();
		LastShot = _currentTime;
	}
}

void BullDefaultState::ExitState() 
{

}

//Wave 1 State
BullWave1State::BullWave1State(BullComponent& _bull)
	:BullState(_bull)
{
}
BullWave1State::~BullWave1State()
{}

void BullWave1State::EnterState() {

}
void BullWave1State::Update() {

}

void BullWave1State::ExitState() {

}

//Wave 2 State
BullWave2State::BullWave2State(BullComponent& _bull)
	:BullState(_bull)
{
}
BullWave2State::~BullWave2State()
{}

void BullWave2State::EnterState() {

}
void BullWave2State::Update() {

}

void BullWave2State::ExitState() {

}
