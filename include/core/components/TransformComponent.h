#pragma once

#include <core/entity/Component.h>
#include <core/math/Vec2.h>
#include <vector>
#include <algorithm>

class GameServiceHost;

// Core component for spatial data: position, direction, scale, rotation.
// Supports parent-child hierarchy with local/world coordinate transforms.
// Local coordinates are relative to parent; world coordinates are absolute.
class TransformComponent : public ActorComponent
{
public:
	TransformComponent(Actor& entity, GameServiceHost& services);
	~TransformComponent() override;

	const char* GetName() const override { return "Transform"; }

	// ========== Hierarchy Management ==========
	
	// Set parent transform. Pass nullptr to detach from parent.
	// Optionally preserve world position when re-parenting.
	void SetParent(TransformComponent* newParent, bool preserveWorldPosition = true);
	TransformComponent* GetParent() const { return Parent; }
	bool HasParent() const { return Parent != nullptr; }

	// Child access (read-only, children managed via SetParent)
	const std::vector<TransformComponent*>& GetChildren() const { return Children; }
	size_t GetChildCount() const { return Children.size(); }
	bool HasChildren() const { return !Children.empty(); }

	// Detach all children (children become root-level, preserving world positions)
	void DetachAllChildren();

	// Check if this transform is a descendant of another
	bool IsDescendantOf(const TransformComponent* ancestor) const;

	// ========== Local Position (relative to parent) ==========
	
	void SetLocalPosition(const Vec2& position) { LocalPosition = position; }
	void SetLocalPosition(float x, float y) { LocalPosition.x = x; LocalPosition.y = y; }
	const Vec2& GetLocalPosition() const { return LocalPosition; }

	// ========== World Position (absolute) ==========
	
	// Set world position - automatically adjusts local offset if parented
	void SetPosition(const Vec2& position);
	void SetPosition(float x, float y);
	
	// Get computed world position (local + parent chain)
	Vec2 GetPosition() const;
	
	// Non-const accessor for compatibility - returns computed world position
	Vec2& GetPosition();

	// ========== Direction (normalized facing direction) ==========
	
	void SetDirection(const Vec2& direction) { Direction = direction; }
	void SetDirection(float x, float y) { Direction.x = x; Direction.y = y; }
	const Vec2& GetDirection() const { return Direction; }
	Vec2& GetDirection() { return Direction; }

	// ========== Scale ==========
	
	void SetLocalScale(const Vec2& scale) { LocalScale = scale; }
	void SetLocalScale(float uniformScale) { LocalScale.x = uniformScale; LocalScale.y = uniformScale; }
	void SetLocalScale(float x, float y) { LocalScale.x = x; LocalScale.y = y; }
	const Vec2& GetLocalScale() const { return LocalScale; }

	// Computed world scale (accumulates parent scales)
	Vec2 GetScale() const;
	
	// Legacy API - sets local scale
	void SetScale(const Vec2& scale) { SetLocalScale(scale); }
	void SetScale(float uniformScale) { SetLocalScale(uniformScale); }
	void SetScale(float x, float y) { SetLocalScale(x, y); }

	// ========== Rotation (in degrees) ==========
	
	void SetLocalRotation(float degrees) { LocalRotation = degrees; }
	float GetLocalRotation() const { return LocalRotation; }

	// Computed world rotation (accumulates parent rotations)
	float GetRotation() const;
	
	// Legacy API - sets local rotation
	void SetRotation(float degrees) { SetLocalRotation(degrees); }

	// ========== Convenience Methods ==========
	
	void Translate(const Vec2& offset) { LocalPosition += offset; }
	void Translate(float x, float y) { LocalPosition.x += x; LocalPosition.y += y; }
	void Rotate(float degrees) { LocalRotation += degrees; }

	// Transform a point from local space to world space
	Vec2 LocalToWorld(const Vec2& localPoint) const;
	
	// Transform a point from world space to local space
	Vec2 WorldToLocal(const Vec2& worldPoint) const;

private:
	void AddChild(TransformComponent* child);
	void RemoveChild(TransformComponent* child);

	// Hierarchy
	TransformComponent* Parent = nullptr;
	std::vector<TransformComponent*> Children;

	// Local transform data (relative to parent, or world if no parent)
	Vec2 LocalPosition = Vec2(0.0f, 0.0f);
	Vec2 Direction = Vec2(1.0f, 0.0f);  // Default facing right
	Vec2 LocalScale = Vec2(1.0f, 1.0f);
	float LocalRotation = 0.0f;

	// Cached world position for non-const GetPosition() accessor
	mutable Vec2 CachedWorldPosition;
};
