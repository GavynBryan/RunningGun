#pragma once
#include <iostream>
#include <SFML/Graphics/Shape.hpp>

namespace VectorMath {
	sf::Vector2f Normalize(const sf::Vector2f& _vec) {
		float length = sqrt((_vec.x * _vec.x) + (_vec.y * _vec.y));
		if (length != 0) {
			return sf::Vector2f(_vec.x, _vec.y)/length;
			
		} return _vec;
	}

	void NormalizeVector(sf::Vector2f& _vec) {
		float length = sqrt((_vec.x * _vec.x) + (_vec.y * _vec.y));
		if (length != 0) {
			_vec = sf::Vector2f(_vec.x / length, _vec.y / length);
		}
	}
}