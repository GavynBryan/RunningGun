#pragma once
#include <functional>

class Timer
{
private:
	float EndTime;
	std::function<void()> Callback;

public:
	Timer(float _endTime, std::function<void()> _callback)
		: EndTime(_endTime), Callback(_callback) {}

	bool IsExpired(float _currentTime) const { return _currentTime >= EndTime; }
	void Fire() { if (Callback) Callback(); }
};
