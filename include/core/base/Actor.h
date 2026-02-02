#pragma once
#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <core/math/Vec2.h>
#include <core/entity/Component.h>

class TransformComponent;
class SpriteComponent;

enum ACTOR_TAG {
	player = 0,
	bullet,
	enemy_bullet,
	hazard,
	pickup
};

class Actor
{
protected:
	std::vector<std::unique_ptr<ActorComponent>> Components;

	ACTOR_TAG			Tag = player;
	bool				Activated = true;

	// Required component - every entity has exactly one transform
	TransformComponent* Transform = nullptr;

public:
	Actor();
	virtual ~Actor();

	typedef std::unique_ptr<Actor> Ptr;

	// Component management
	void AttachComponent(std::unique_ptr<ActorComponent> comp);

	template <typename Comp>
	Comp* GetComponent();

	template <typename Comp>
	const Comp* GetComponent() const;

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
	Transform* GetTransform() { return Transform; }
	const Transform* GetTransform() const { return Transform; }

	// Convenience position/direction accessors that delegate to Transform
	void SetPosition(const Vec2& pos);
	void SetPosition(float x, float y);
	Vec2 GetPosition() const;

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
};

template<typename Comp>
Comp* Actor::GetComponent() {
	for (auto& component : Components) {
		if (auto* result = dynamic_cast<Comp*>(component.get())) {
			return result;
		}
	}
	return nullptr;
}

template<typename Comp>
const Comp* Actor::GetComponent() const {
	for (const auto& component : Components) {
		if (const auto* result = dynamic_cast<const Comp*>(component.get())) {
			return result;
		}
	}
	return nullptr;
}
