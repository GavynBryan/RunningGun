#pragma once
#include <functional>

class Timer
{
private:
	bool Active;
	float EndTime;
	std::function<void()> Callback;

public:
	Timer() : Active(false), EndTime(0), Callback(nullptr) {}

	void Start(float _duration, std::function<void()> _callback);
	void Update(float _currentTime);
	void Cancel();
	bool IsActive() const { return Active; }
};
