#pragma once
#include "Vec2.h"

class sPhysics
{
private:
	Vec2f				gGravity;
	float				gGroundY;

public:
	sPhysics();
	~sPhysics();

	float				getGroundLevel() { return gGroundY; }
	Vec2f				getGravity() { return gGravity; }
};
