#pragma once
#include <iostream>
#include <cmath>
#include <core/Vec2.h>

namespace VectorMath {
	inline Vec2 Normalize(const Vec2& _vec) {
		float length = std::sqrt((_vec.x * _vec.x) + (_vec.y * _vec.y));
		if (length != 0) {
			return Vec2(_vec.x / length, _vec.y / length);
		}
		return _vec;
	}

	inline void NormalizeVector(Vec2& _vec) {
		float length = std::sqrt((_vec.x * _vec.x) + (_vec.y * _vec.y));
		if (length != 0) {
			_vec = Vec2(_vec.x / length, _vec.y / length);
		}
	}
}
