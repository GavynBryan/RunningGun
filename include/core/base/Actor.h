#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <typeindex>
#include <unordered_map>
#include <core/math/Vec2.h>
#include <core/data/Transform2D.h>
#include <core/entity/Component.h>
#include <core/events/MulticastDelegate.h>

class SpriteComponent;

enum ACTOR_TAG {
	player = 0,
	bullet,
	enemy_bullet,
	hazard,
	pickup
};

// Type alias for component storage
using ComponentList = std::vector<std::unique_ptr<ActorComponent>>;

//=============================================================================
// Actor
// 
// Base class for all game objects in the scene. Acts as a scene node with
// embedded transform data for spatial hierarchy. Components add behavior.
// 
// Transform data is stored directly (not as a component) because every Actor
// must exist in world space. This simplifies the architecture and enables
// future ECS hybrid approaches where Transform2D can be shared.
//=============================================================================
class Actor
{
protected:
	// Type-indexed map owns all components - O(1) lookup by type
	std::unordered_map<std::type_index, ComponentList> ComponentsByType;

	// Flat list for iteration order (Start/Update) - raw pointers, not owning
	std::vector<ActorComponent*> AllComponents;

	ACTOR_TAG Tag = player;
	bool Activated = true;

	// ========== Embedded Transform Data ==========
	// Local transform (relative to parent, or world if no parent)
	Transform2D LocalTransform;
	Vec2 Direction = Vec2(1.0f, 0.0f);  // Normalized facing direction

	// Cached world transform for non-const accessors
	mutable Vec2 CachedWorldPosition;
	mutable Transform2D CachedWorldTransform;

	// Scene hierarchy (non-owning pointers - ActorService owns all actors)
	Actor* Parent = nullptr;
	std::vector<Actor*> Children;

public:
	Actor();
	virtual ~Actor();

	typedef std::unique_ptr<Actor> Ptr;

	// ========== Hierarchy Management ==========

	// Set parent actor. Pass nullptr to detach.
	// preserveWorldPosition: if true, adjusts local transform to maintain world position
	void SetParent(Actor* newParent, bool preserveWorldPosition = true);
	Actor* GetParent() const { return Parent; }
	bool HasParent() const { return Parent != nullptr; }

	// Child access (read-only, children managed via SetParent)
	const std::vector<Actor*>& GetChildren() const { return Children; }
	size_t GetChildCount() const { return Children.size(); }
	bool HasChildren() const { return !Children.empty(); }

	// Detach all children (children become root actors, preserving world positions)
	void DetachAllChildren();

	// Check if this actor is a descendant of another
	bool IsDescendantOf(const Actor* ancestor) const;

	// Recursively enable/disable actor and all descendants
	void SetEnabledRecursive(bool enabled);

	// Event fired when hierarchy changes (parent set, child added/removed)
	MulticastDelegate<> OnHierarchyChanged;

	// ========== Component Management ==========

	void AttachComponent(std::unique_ptr<ActorComponent> comp);

	// Remove a specific component by reference
	bool RemoveComponent(ActorComponent& comp);

	// Event broadcast when components are added/removed
	MulticastDelegate<> OnComponentsChanged;

	// Remove all components of a type
	template <typename Comp>
	size_t RemoveComponents();

	template <typename Comp>
	Comp* GetComponent();

	template <typename Comp>
	const Comp* GetComponent() const;

	// Get all components of a type (for actors with multiple components of same type)
	template <typename Comp>
	std::vector<Comp*> GetComponents();

	template <typename Comp>
	std::vector<const Comp*> GetComponents() const;

	// Lifecycle
	virtual void Start();
	virtual void Update();

	void StartComponents();
	void UpdateComponents();

	// State
	void SetTag(ACTOR_TAG tag) { Tag = tag; }
	ACTOR_TAG GetTag() const { return Tag; }

	void Enable() { Activated = true; Start(); }
	void Disable() { Activated = false; }
	bool IsEnabled() const { return Activated; }

	// ========== Transform - Local (relative to parent) ==========

	void SetLocalPosition(const Vec2& position) { LocalTransform.Position = position; }
	void SetLocalPosition(float x, float y) { LocalTransform.Position.x = x; LocalTransform.Position.y = y; }
	const Vec2& GetLocalPosition() const { return LocalTransform.Position; }

	void SetLocalScale(const Vec2& scale) { LocalTransform.Scale = scale; }
	void SetLocalScale(float uniformScale) { LocalTransform.Scale.x = uniformScale; LocalTransform.Scale.y = uniformScale; }
	void SetLocalScale(float x, float y) { LocalTransform.Scale.x = x; LocalTransform.Scale.y = y; }
	const Vec2& GetLocalScale() const { return LocalTransform.Scale; }

	void SetLocalRotation(float degrees) { LocalTransform.Rotation = degrees; }
	float GetLocalRotation() const { return LocalTransform.Rotation; }

	// Direct access to local transform struct
	const Transform2D& GetLocalTransform() const { return LocalTransform; }
	void SetLocalTransform(const Transform2D& transform) { LocalTransform = transform; }

	// ========== Transform - World (absolute) ==========

	// Set world position - automatically adjusts local offset if parented
	void SetPosition(const Vec2& position);
	void SetPosition(float x, float y);
	Vec2 GetPosition() const;
	Vec2& GetPosition();  // Non-const accessor (uses cached value)

	// Computed world scale (accumulates parent scales)
	Vec2 GetScale() const;

	// Computed world rotation (accumulates parent rotations)
	float GetRotation() const;

	// Get world transform (computed from hierarchy)
	Transform2D GetTransform() const;
	Transform2D& GetTransform();  // Non-const accessor (uses cached value)

	// ========== Direction ==========

	void SetDirection(const Vec2& direction) { Direction = direction; }
	void SetDirection(float x, float y) { Direction.x = x; Direction.y = y; }
	const Vec2& GetDirection() const { return Direction; }
	Vec2& GetDirection() { return Direction; }

	// ========== Transform Utilities ==========

	void Translate(const Vec2& offset) { LocalTransform.Position += offset; }
	void Translate(float x, float y) { LocalTransform.Position.x += x; LocalTransform.Position.y += y; }
	void Rotate(float degrees) { LocalTransform.Rotation += degrees; }

	// Transform a point from local space to world space
	Vec2 LocalToWorld(const Vec2& localPoint) const;

	// Transform a point from world space to local space
	Vec2 WorldToLocal(const Vec2& worldPoint) const;

	// ========== Sprite Convenience ==========

	// Sprite access (optional - returns nullptr if no SpriteComponent)
	SpriteComponent* GetSprite() { return GetComponent<SpriteComponent>(); }
	const SpriteComponent* GetSprite() const { return GetComponent<SpriteComponent>(); }

	// Sprite flip convenience (no-op if no sprite)
	void SetFlipX(bool flip);
	bool GetFlipX() const;

	// Size accessors
	float GetWidth() const;
	float GetHeight() const;

	// Collision callback
	void OnCollide(Actor& other);

private:
	void AddChild(Actor* child);
	void RemoveChild(Actor* child);
};

template<typename Comp>
Comp* Actor::GetComponent() {
	auto it = ComponentsByType.find(std::type_index(typeid(Comp)));
	if (it != ComponentsByType.end() && !it->second.empty()) {
		return static_cast<Comp*>(it->second.front().get());
	}
	return nullptr;
}

template<typename Comp>
const Comp* Actor::GetComponent() const {
	auto it = ComponentsByType.find(std::type_index(typeid(Comp)));
	if (it != ComponentsByType.end() && !it->second.empty()) {
		return static_cast<const Comp*>(it->second.front().get());
	}
	return nullptr;
}

template<typename Comp>
std::vector<Comp*> Actor::GetComponents() {
	std::vector<Comp*> results;
	auto it = ComponentsByType.find(std::type_index(typeid(Comp)));
	if (it != ComponentsByType.end()) {
		results.reserve(it->second.size());
		for (auto& comp : it->second) {
			results.push_back(static_cast<Comp*>(comp.get()));
		}
	}
	return results;
}

template<typename Comp>
std::vector<const Comp*> Actor::GetComponents() const {
	std::vector<const Comp*> results;
	auto it = ComponentsByType.find(std::type_index(typeid(Comp)));
	if (it != ComponentsByType.end()) {
		results.reserve(it->second.size());
		for (const auto& comp : it->second) {
			results.push_back(static_cast<const Comp*>(comp.get()));
		}
	}
	return results;
}

template<typename Comp>
size_t Actor::RemoveComponents() {
	std::type_index typeIdx(typeid(Comp));
	auto it = ComponentsByType.find(typeIdx);
	if (it == ComponentsByType.end()) {
		return 0;
	}

	size_t count = it->second.size();

	// Remove from AllComponents list
	for (auto& comp : it->second) {
		ActorComponent* rawPtr = comp.get();
		AllComponents.erase(
			std::remove(AllComponents.begin(), AllComponents.end(), rawPtr),
			AllComponents.end()
		);
	}

	// Remove the entire type bucket
	ComponentsByType.erase(it);

	// Notify subscribers of component change
	OnComponentsChanged.Broadcast();

	return count;
}
