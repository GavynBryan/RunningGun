#pragma once

#include <core/math/Vec2.h>

//=============================================================================
// Transform2D
// 
// Lightweight struct for 2D transform data: position, scale, rotation.
// Core data type shared between component-based and ECS architectures.
// Used by rendering, physics, and any system needing spatial data.
//=============================================================================
struct Transform2D
{
	Vec2 Position = { 0.0f, 0.0f };
	Vec2 Scale = { 1.0f, 1.0f };
	float Rotation = 0.0f;  // Degrees

	Transform2D() = default;
	Transform2D(const Vec2& pos) : Position(pos) {}
	Transform2D(const Vec2& pos, const Vec2& scale) : Position(pos), Scale(scale) {}
	Transform2D(const Vec2& pos, const Vec2& scale, float rot) : Position(pos), Scale(scale), Rotation(rot) {}
	Transform2D(float x, float y) : Position(x, y) {}
};
