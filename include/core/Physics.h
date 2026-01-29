#pragma once
#include <core/Vec2.h>

class Physics
{
private:
	Vec2				Gravity;
	float				GroundY;

public:
	Physics();
	~Physics();

	float				GetGroundLevel() { return GroundY; }
	Vec2				GetGravity() { return Gravity; }
};
