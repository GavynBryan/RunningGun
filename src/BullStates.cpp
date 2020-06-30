#include <BullStates.h>
#include <BullComponent.h>



BullState::BullState(BullComponent& _bull)
	:mBull(_bull)
{
}

BullState::~BullState()
{}

void BullState::enterState(){}
void BullState::update(){}
void BullState::postUpdate(){}
void BullState::exitState(){}


//Default State
BullDefaultState::BullDefaultState(BullComponent& _bull)
	:BullState(_bull),
	lastShot(0)
{
}
BullDefaultState::~BullDefaultState()
{}

void BullDefaultState::enterState() 
{
	lastShot = 0;
}

void BullDefaultState::update() 
{
	float currentTime = sEnvironment::Instance().getElapsedTime();
	if (currentTime - lastShot > shootFrequency) {
		mBull.shoot();
		lastShot = currentTime;
	}
}

void BullDefaultState::postUpdate() 
{

}

void BullDefaultState::exitState() 
{

}

//Wave 1 State
BullWave1State::BullWave1State(BullComponent& _bull)
	:BullState(_bull)
{
}
BullWave1State::~BullWave1State()
{}

void BullWave1State::enterState() {

}
void BullWave1State::update() {

}

void BullWave1State::postUpdate() {

}

void BullWave1State::exitState() {

}

//Wave 2 State
BullWave2State::BullWave2State(BullComponent& _bull)
	:BullState(_bull)
{
}
BullWave2State::~BullWave2State()
{}

void BullWave2State::enterState() {

}
void BullWave2State::update() {

}

void BullWave2State::postUpdate() {

}

void BullWave2State::exitState() {

}
