#pragma once

class GameMode
{
public:
	virtual						~GameMode() = default;

	virtual void				Init() = 0;
	virtual void				BuildScene() = 0;
	virtual void				Update() = 0;
	virtual void				PostUpdate() = 0;
	virtual void				RequestReset() = 0;
};
