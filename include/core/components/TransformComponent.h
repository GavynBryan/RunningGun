#pragma once

#include <core/entity/Component.h>
#include <core/math/Vec2.h>

class GameServiceHost;

// Core component for spatial data: position, direction, scale, rotation
// Decoupled from Entity to allow component-based architecture
class TransformComponent : public ActorComponent
{
public:
	TransformComponent(Actor& entity, GameServiceHost& services);
	~TransformComponent() override = default;

	const char* GetName() const override { return "Transform"; }

	// Position
	void SetPosition(const Vec2& position) { Position = position; }
	void SetPosition(float x, float y) { Position.x = x; Position.y = y; }
	const Vec2& GetPosition() const { return Position; }
	Vec2& GetPosition() { return Position; }

	// Direction (normalized facing direction)
	void SetDirection(const Vec2& direction) { Direction = direction; }
	void SetDirection(float x, float y) { Direction.x = x; Direction.y = y; }
	const Vec2& GetDirection() const { return Direction; }
	Vec2& GetDirection() { return Direction; }

	// Scale
	void SetScale(const Vec2& scale) { Scale = scale; }
	void SetScale(float uniformScale) { Scale.x = uniformScale; Scale.y = uniformScale; }
	void SetScale(float x, float y) { Scale.x = x; Scale.y = y; }
	const Vec2& GetScale() const { return Scale; }

	// Rotation (in degrees)
	void SetRotation(float degrees) { Rotation = degrees; }
	float GetRotation() const { return Rotation; }

	// Convenience methods
	void Translate(const Vec2& offset) { Position += offset; }
	void Translate(float x, float y) { Position.x += x; Position.y += y; }
	void Rotate(float degrees) { Rotation += degrees; }

private:
	Vec2 Position = Vec2(0.0f, 0.0f);
	Vec2 Direction = Vec2(1.0f, 0.0f);  // Default facing right
	Vec2 Scale = Vec2(1.0f, 1.0f);
	float Rotation = 0.0f;
};
