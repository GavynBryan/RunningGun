#pragma once

#include <core/math/Vec2.h>
#include <cmath>

//=============================================================================
// Transform2D
// 
// Lightweight POD struct for 2D transform data: position, scale, rotation.
// Designed for data-oriented patterns - can be used standalone or with ECS.
// Contains transform math operations for composing/decomposing hierarchies.
//=============================================================================
struct Transform2D
{
	Vec2 Position = { 0.0f, 0.0f };
	Vec2 Scale = { 1.0f, 1.0f };
	float Rotation = 0.0f;  // Degrees

	// ========== Constructors ==========

	Transform2D() = default;
	Transform2D(const Vec2& pos) : Position(pos) {}
	Transform2D(const Vec2& pos, const Vec2& scale) : Position(pos), Scale(scale) {}
	Transform2D(const Vec2& pos, const Vec2& scale, float rot) : Position(pos), Scale(scale), Rotation(rot) {}
	Transform2D(float x, float y) : Position(x, y) {}

	// ========== Transform Operations ==========

	// Transform a local point to world space using this transform
	Vec2 LocalToWorld(const Vec2& localPoint) const
	{
		// Apply scale
		Vec2 scaled(localPoint.x * Scale.x, localPoint.y * Scale.y);

		// Apply rotation
		float radians = Rotation * 3.14159265358979f / 180.0f;
		float cosR = std::cos(radians);
		float sinR = std::sin(radians);
		Vec2 rotated(
			scaled.x * cosR - scaled.y * sinR,
			scaled.x * sinR + scaled.y * cosR
		);

		// Apply translation
		return rotated + Position;
	}

	// Transform a world point to local space using this transform
	Vec2 WorldToLocal(const Vec2& worldPoint) const
	{
		// Reverse translation
		Vec2 point = worldPoint - Position;

		// Reverse rotation
		float radians = -Rotation * 3.14159265358979f / 180.0f;
		float cosR = std::cos(radians);
		float sinR = std::sin(radians);
		Vec2 unrotated(
			point.x * cosR - point.y * sinR,
			point.x * sinR + point.y * cosR
		);

		// Reverse scale
		return Vec2(
			Scale.x != 0.0f ? unrotated.x / Scale.x : 0.0f,
			Scale.y != 0.0f ? unrotated.y / Scale.y : 0.0f
		);
	}

	// Compose two transforms (this * other) - applies other in this's local space
	Transform2D Compose(const Transform2D& child) const
	{
		Transform2D result;
		result.Position = LocalToWorld(child.Position);
		result.Scale = Vec2(Scale.x * child.Scale.x, Scale.y * child.Scale.y);
		result.Rotation = Rotation + child.Rotation;
		return result;
	}

	// Get the inverse transform
	Transform2D Inverse() const
	{
		Transform2D result;
		
		// Inverse scale
		result.Scale = Vec2(
			Scale.x != 0.0f ? 1.0f / Scale.x : 0.0f,
			Scale.y != 0.0f ? 1.0f / Scale.y : 0.0f
		);
		
		// Inverse rotation
		result.Rotation = -Rotation;
		
		// Inverse position (needs to account for rotation and scale)
		float radians = -Rotation * 3.14159265358979f / 180.0f;
		float cosR = std::cos(radians);
		float sinR = std::sin(radians);
		Vec2 rotatedPos(
			-Position.x * cosR + Position.y * sinR,
			-Position.x * sinR - Position.y * cosR
		);
		result.Position = Vec2(rotatedPos.x * result.Scale.x, rotatedPos.y * result.Scale.y);
		
		return result;
	}

	// ========== Utility ==========

	// Linearly interpolate between two transforms
	static Transform2D Lerp(const Transform2D& a, const Transform2D& b, float t)
	{
		return Transform2D(
			Vec2(a.Position.x + (b.Position.x - a.Position.x) * t,
			     a.Position.y + (b.Position.y - a.Position.y) * t),
			Vec2(a.Scale.x + (b.Scale.x - a.Scale.x) * t,
			     a.Scale.y + (b.Scale.y - a.Scale.y) * t),
			a.Rotation + (b.Rotation - a.Rotation) * t
		);
	}

	// Identity transform
	static Transform2D Identity()
	{
		return Transform2D();
	}

	// Check if this is approximately an identity transform
	bool IsIdentity(float epsilon = 0.0001f) const
	{
		return std::abs(Position.x) < epsilon &&
		       std::abs(Position.y) < epsilon &&
		       std::abs(Scale.x - 1.0f) < epsilon &&
		       std::abs(Scale.y - 1.0f) < epsilon &&
		       std::abs(Rotation) < epsilon;
	}

	// ========== Operators ==========

	bool operator==(const Transform2D& other) const
	{
		return Position.x == other.Position.x &&
		       Position.y == other.Position.y &&
		       Scale.x == other.Scale.x &&
		       Scale.y == other.Scale.y &&
		       Rotation == other.Rotation;
	}

	bool operator!=(const Transform2D& other) const
	{
		return !(*this == other);
	}
};
