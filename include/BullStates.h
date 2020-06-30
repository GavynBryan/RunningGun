#pragma once
#include <core/State.h>

class BullComponent;

class BullState :
	public State
{
protected:
	BullComponent &mBull;
public:
	BullState(BullComponent& _bull);
	~BullState();

	virtual void enterState();
	virtual void update();
	virtual void postUpdate();
	virtual void exitState();
};

//Default state: Can take 10 hits of damage, shouting at you.
//After 10 hits, he spawn scorpions. Invincible for 10 seconds until he's back to his default state
//10 more hits, then he starts rolling barrels towards you
class BullDefaultState
	:public BullState
{
private:
	//timestamp
	float lastShot;
	float shootFrequency = 1.3f;
public:
	BullDefaultState(BullComponent& _bull);
	~BullDefaultState();
	void enterState();
	void update();
	void postUpdate();
	void exitState();
};

class BullWave1State
	:public BullState
{
private:
	float timestamp;
public:
	BullWave1State(BullComponent& _bull);
	~BullWave1State();
	void enterState();
	void update();
	void postUpdate();
	void exitState();

	//void damage();
};


class BullWave2State
	:public BullState
{
private:
	float timestamp;
public:
	BullWave2State(BullComponent& _bull);
	~BullWave2State();
	void enterState();
	void update();
	void postUpdate();
	void exitState();

	//void damage();
};
