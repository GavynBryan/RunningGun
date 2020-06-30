#pragma once
class State
{
private:
	bool active;
public:
	State();
	~State();

	virtual void	enterState();
	virtual void	update();
	virtual void	postUpdate();
	virtual void	exitState();
};

