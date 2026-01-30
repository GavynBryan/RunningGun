#pragma once

class FrameClock
{
private:
	float DeltaTimeValue;

public:
	FrameClock();

	void SetDeltaTime(float _deltaTime);
	float DeltaTime() const;
};
