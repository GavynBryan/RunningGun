#pragma once
#include <SFML/Graphics/Shape.hpp>
class sPhysics
{
private:
	sf::Vector2f		gGravity;
	float				gGroundY;

public:
	sPhysics();
	~sPhysics();

	float				getGroundLevel() { return gGroundY; }
	sf::Vector2f		getGravity() { return gGravity; }
};

