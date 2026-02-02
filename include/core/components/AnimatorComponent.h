#pragma once
#include <core/entity/Component.h>
#include <core/animation/AnimationController.h>
#include <memory>
#include <string_view>

class SpriteComponent;
class TimeService;

// AnimatorComponent - Component wrapper for AnimationController.
//
// This component owns and manages an AnimationController, providing
// a clean interface for entities to control their animations.
//
// The component automatically finds and caches the SpriteComponent
// on Start(), and updates the controller each frame with delta time.
//
// Usage:
//   auto animator = std::make_unique<AnimatorComponent>(entity, services);
//   animator->GetController().AddClip("idle", idleClip);
//   entity.AttachComponent(std::move(animator));
//
//   // Later, in gameplay code:
//   auto* animator = entity.GetComponent<AnimatorComponent>();
//   animator->SetState("walk");
//   animator->PlayOneShot("shoot");
//
class AnimatorComponent : public ActorComponent
{
public:
	AnimatorComponent(Actor& entity, GameServiceHost& services);
	~AnimatorComponent() override;

	const char* GetName() const override { return "AnimatorComponent"; }

	// Lifecycle
	void Start() override;
	void Update() override;

	// Controller access - for setup and advanced control
	AnimationController& GetController() { return Controller; }
	const AnimationController& GetController() const { return Controller; }

	// Convenience methods that delegate to Controller
	void SetState(std::string_view stateName) { Controller.SetState(stateName); }
	void PlayOneShot(std::string_view clipName, bool forceInterrupt = false) { Controller.PlayOneShot(clipName, forceInterrupt); }
	void PlayAnimation(std::string_view name) { Controller.PlayAnimation(name); }

	std::string_view GetCurrentState() const { return Controller.GetCurrentState(); }
	bool IsPlayingOneShot() const { return Controller.IsPlayingOneShot(); }

private:
	AnimationController Controller;
	SpriteComponent* CachedSprite = nullptr;
	TimeService* Time = nullptr;
};
