#pragma once
#include <core/State.h>

class PlayerComponent;

class PlayerState :
	public State
{
protected:
	PlayerComponent& mPlayer;
public:
	PlayerState(PlayerComponent& _player);
	~PlayerState();

	virtual void enterState();
	virtual void update();
	virtual void postUpdate();
	virtual void exitState();

	virtual void damage();
};

class defaultPlayerState
	:public PlayerState
{
public:
	defaultPlayerState(PlayerComponent& _player);
	~defaultPlayerState();

	void enterState();
	void update();
	void postUpdate();
	void exitState();

	void damage();
};

class damagePlayerState
	:public PlayerState
{
private:
	float	timestamp;
	float	stateLength = 1.0f;
public:
	damagePlayerState(PlayerComponent& _player);
	~damagePlayerState();

	void enterState();
	void update();

};

class deadPlayerState
	:public PlayerState
{
private:
	float	timestamp;
	float	stateLength = 5.0f;
public:
	deadPlayerState(PlayerComponent& _player);
	~deadPlayerState();

	void enterState();
	void update();

};

class victoryPlayerState
	:public PlayerState
{
public:
	victoryPlayerState(PlayerComponent& _player);
	~victoryPlayerState();

	void enterState();

};