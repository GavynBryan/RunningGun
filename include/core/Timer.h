#pragma once
#include <functional>
#include <cstdint>

using TimerHandle = uint32_t;
constexpr TimerHandle InvalidTimerHandle = 0;

class Timer
{
private:
	TimerHandle Handle;
	float EndTime;
	std::function<void()> Callback;

public:
	Timer(TimerHandle _handle, float _endTime, std::function<void()> _callback)
		: Handle(_handle), EndTime(_endTime), Callback(_callback) {}

	TimerHandle GetHandle() const { return Handle; }
	bool IsExpired(float _currentTime) const { return _currentTime >= EndTime; }
	void Fire() { if (Callback) Callback(); }
};
