#pragma once
#include <cmath>

namespace MathUtils {
inline float MoveToward(float current, float target, float maxDelta)
{
	if (std::abs(target - current) <= maxDelta) {
		return target;
	}
	return current + (target > current ? maxDelta : -maxDelta);
}
}
