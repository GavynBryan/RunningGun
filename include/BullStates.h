#pragma once
#include <core/State.h>

class BullComponent;

class BullState :
	public State
{
protected:
	BullComponent &BullRef;
public:
	BullState(BullComponent& _bull);
	~BullState();

	virtual void EnterState();
	virtual void Update();
	virtual void PostUpdate();
	virtual void ExitState();
};

//Default state: Can take 10 hits of damage, shouting at you.
//After 10 hits, he spawn scorpions. Invincible for 10 seconds until he's back to his default state
//10 more hits, then he starts rolling barrels towards you
class BullDefaultState
	:public BullState
{
private:
	//timestamp
	float LastShot;
	float ShootFrequency = 1.3f;
public:
	BullDefaultState(BullComponent& _bull);
	~BullDefaultState();
	void EnterState();
	void Update();
	void PostUpdate();
	void ExitState();
};

class BullWave1State
	:public BullState
{
private:
	float Timestamp;
public:
	BullWave1State(BullComponent& _bull);
	~BullWave1State();
	void EnterState();
	void Update();
	void PostUpdate();
	void ExitState();

	//void damage();
};


class BullWave2State
	:public BullState
{
private:
	float Timestamp;
public:
	BullWave2State(BullComponent& _bull);
	~BullWave2State();
	void EnterState();
	void Update();
	void PostUpdate();
	void ExitState();

	//void damage();
};
