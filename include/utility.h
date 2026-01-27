#pragma once
#include <iostream>
#include <cmath>
#include <core/Vec2.h>

namespace VectorMath {
	inline Vec2 Normalize(const Vec2& _vec) {
		float _length = std::sqrt((_vec.x * _vec.x) + (_vec.y * _vec.y));
		if (_length != 0) {
			return Vec2(_vec.x / _length, _vec.y / _length);
		}
		return _vec;
	}

	inline void NormalizeVector(Vec2& _vec) {
		float _length = std::sqrt((_vec.x * _vec.x) + (_vec.y * _vec.y));
		if (_length != 0) {
			_vec = Vec2(_vec.x / _length, _vec.y / _length);
		}
	}
}
