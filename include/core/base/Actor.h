#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <typeindex>
#include <unordered_map>
#include <core/math/Vec2.h>
#include <core/math/Transform2D.h>
#include <core/entity/Component.h>
#include <core/events/MulticastDelegate.h>

class TransformComponent;
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

class Actor
{
protected:
	// Type-indexed map owns all components - O(1) lookup by type
	std::unordered_map<std::type_index, ComponentList> ComponentsByType;

	// Flat list for iteration order (Start/Update) - raw pointers, not owning
	std::vector<ActorComponent*> AllComponents;

	ACTOR_TAG			Tag = player;
	bool				Activated = true;

	// Required component - every entity has exactly one transform
	TransformComponent* Transform = nullptr;

	// Actor hierarchy (non-owning pointers - ActorService owns all actors)
	Actor* Parent = nullptr;
	std::vector<Actor*> Children;

public:
	Actor();
	virtual ~Actor();

	typedef std::unique_ptr<Actor> Ptr;

	// ========== Hierarchy Management ==========

	// Set parent actor. Pass nullptr to detach.
	// Updates both Actor hierarchy and underlying Transform hierarchy.
	// preserveWorldPosition: if true, adjusts local position to maintain world position
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

	// Transform access - guaranteed to exist after construction
	TransformComponent* GetTransformComponent() { return Transform; }
	const TransformComponent* GetTransformComponent() const { return Transform; }

	// Get transform data as value type (for rendering/ECS interop)
	Transform2D GetTransform() const;

	// Convenience position/direction accessors that delegate to Transform
	// World position (absolute position in scene)
	void SetPosition(const Vec2& pos);
	void SetPosition(float x, float y);
	Vec2 GetPosition() const;

	// Local position (relative to parent, or world if no parent)
	void SetLocalPosition(const Vec2& pos);
	void SetLocalPosition(float x, float y);
	Vec2 GetLocalPosition() const;

	void SetDirection(const Vec2& dir);
	void SetDirection(float x, float y);
	Vec2 GetDirection() const;

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
	void EnsureTransform();
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

		// Invalidate Transform cache if needed
		if (rawPtr == Transform) {
			Transform = nullptr;
		}
	}

	// Remove the entire type bucket
	ComponentsByType.erase(it);

	// Notify subscribers of component change
	OnComponentsChanged.Broadcast();

	return count;
}
