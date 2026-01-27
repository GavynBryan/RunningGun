#pragma once
class State
{
private:
	bool Active;
public:
	State();
	~State();

	virtual void	EnterState();
	virtual void	Update();
	virtual void	PostUpdate();
	virtual void	ExitState();
};

