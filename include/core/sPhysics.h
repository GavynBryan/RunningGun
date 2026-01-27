#pragma once
#include "Vec2.h"

class sPhysics
{
private:
	Vec2				gGravity;
	float				gGroundY;

public:
	sPhysics();
	~sPhysics();

	float				getGroundLevel() { return gGroundY; }
	Vec2				getGravity() { return gGravity; }
};
