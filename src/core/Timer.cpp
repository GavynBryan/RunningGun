#include <core/Timer.h>
#include <core/Environment.h>

void Timer::Start(float _duration, std::function<void()> _callback)
{
	Active = true;
	EndTime = Environment::Instance().GetElapsedTime() + _duration;
	Callback = _callback;
}

void Timer::Update(float _currentTime)
{
	if (Active && _currentTime >= EndTime) {
		Active = false;
		if (Callback) {
			Callback();
		}
	}
}

void Timer::Cancel()
{
	Active = false;
	Callback = nullptr;
}
