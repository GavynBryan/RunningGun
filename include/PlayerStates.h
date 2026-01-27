#pragma once
#include <core/State.h>

class PlayerComponent;

class PlayerState :
	public State
{
protected:
	PlayerComponent& PlayerRef;
public:
	PlayerState(PlayerComponent& _player);
	~PlayerState();

	virtual void EnterState();
	virtual void Update();
	virtual void PostUpdate();
	virtual void ExitState();

	virtual void Damage();
};

class DefaultPlayerState
	:public PlayerState
{
public:
	DefaultPlayerState(PlayerComponent& _player);
	~DefaultPlayerState();

	void EnterState();
	void Update();
	void PostUpdate();
	void ExitState();

	void Damage();
};

class DamagePlayerState
	:public PlayerState
{
private:
	float	Timestamp;
	float	StateLength = 1.0f;
public:
	DamagePlayerState(PlayerComponent& _player);
	~DamagePlayerState();

	void EnterState();
	void Update();

};

class DeadPlayerState
	:public PlayerState
{
private:
	float	Timestamp;
	float	StateLength = 5.0f;
public:
	DeadPlayerState(PlayerComponent& _player);
	~DeadPlayerState();

	void EnterState();
	void Update();

};

class VictoryPlayerState
	:public PlayerState
{
public:
	VictoryPlayerState(PlayerComponent& _player);
	~VictoryPlayerState();

	void EnterState();

};
