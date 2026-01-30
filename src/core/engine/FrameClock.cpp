#include <core/engine/FrameClock.h>

FrameClock::FrameClock()
	: DeltaTimeValue(0.0f)
{
}

void FrameClock::SetDeltaTime(float _deltaTime)
{
	DeltaTimeValue = _deltaTime;
}

float FrameClock::DeltaTime() const
{
	return DeltaTimeValue;
}
