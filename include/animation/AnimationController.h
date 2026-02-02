#pragma once
#include <core/animation/AnimationClip.h>
#include <core/events/MulticastDelegate.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <memory>

class SpriteComponent;

// Animation Controller - A proper state machine for animations.
//
// Design Philosophy:
// - "States" are looping animations (idle, walk, run) - they persist until explicitly changed
// - "One-shots" are non-looping animations (shoot, damage, die) - they play over the current state
//   and automatically return to the state when finished
//
// This design allows gameplay code to set the animation state ("idle" or "walking") 
// and separately trigger one-shots ("shoot") without manually managing the return transition.
//
// Clip Storage:
// - Clips can be owned (AddClip) or referenced from a shared library (AddClipRef)
// - Referenced clips are ideal for duplicate NPCs sharing the same animation data
// - Owned clips are useful for unique or procedurally generated animations
//
// Usage:
//   // Option 1: Own the clip data
//   controller.AddClip("idle", clipData);
//
//   // Option 2: Reference shared clip from library (preferred for NPCs)
//   controller.AddClipRef("idle", library.FindClip("scorpion", "idle"));
//   
//   controller.SetState("idle");           // Base state
//   controller.PlayOneShot("shoot");       // Plays over idle, returns to idle when done
//
class AnimationController
{
public:
	AnimationController();
	~AnimationController();

	// Clip management - owned clips (controller takes a copy)
	void AddClip(const std::string& name, AnimationClipData clip);
	void AddClip(const std::string& name, AnimationClipPtr clip);

	// Clip management - referenced clips (shared, e.g., from AnimationClipLibrary)
	// The referenced clip must outlive this controller.
	void AddClipRef(const std::string& name, const AnimationClipData* clip);

	const AnimationClipData* FindClip(std::string_view name) const;
	bool HasClip(std::string_view name) const;

	// State control - for looping "state" animations
	// Setting a state changes the base animation that plays when no one-shot is active
	void SetState(std::string_view stateName);
	std::string_view GetCurrentState() const { return CurrentStateName; }

	// One-shot control - for non-looping "action" animations
	// One-shots play over the current state and return to it when finished
	// If a one-shot is already playing, the new one-shot will only play if:
	//   - forceInterrupt is true, OR
	//   - the new one-shot has higher or equal priority (future enhancement)
	void PlayOneShot(std::string_view clipName, bool forceInterrupt = false);
	bool IsPlayingOneShot() const { return OneShotActive; }
	std::string_view GetCurrentOneShot() const { return OneShotActive ? CurrentOneShotName : ""; }

	// Legacy API compatibility - plays as state if looping, one-shot if not
	void PlayAnimation(std::string_view name);

	// Update
	void Update(float deltaTime, SpriteComponent& sprite);

	// Events
	MulticastDelegate<std::string_view> OnOneShotFinished;
	MulticastDelegate<std::string_view, std::string_view> OnStateChanged; // (oldState, newState)

private:
	void TransitionToState(const AnimationClipData* clip, std::string_view name);
	void StartOneShot(const AnimationClipData* clip, std::string_view name);
	void FinishOneShot();

	// Owned clips (stored by value)
	std::unordered_map<std::string, AnimationClipData> OwnedClips;
	
	// All clip references (points to either OwnedClips or external library)
	std::unordered_map<std::string, const AnimationClipData*> ClipRefs;

	// State playback (the base/looping animation)
	std::string CurrentStateName;
	AnimationPlayback StatePlayback;

	// One-shot playback (overlays state temporarily)
	std::string CurrentOneShotName;
	AnimationPlayback OneShotPlayback;
	bool OneShotActive = false;
};
