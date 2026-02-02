#pragma once
#include <memory>
#include <string>
#include <core/math/Vec2.h>

class SpriteComponent;

// Represents an animation clip - the raw data for a single animation sequence.
// This is a pure data object designed to be serializable.
// The AnimationController handles playback state and logic.
struct AnimationClipData
{
	std::string Name;
	Vec2 FrameSize = Vec2(0.0f, 0.0f);
	int RowIndex = 0;          // Which row in the spritesheet
	int FrameCount = 0;        // Number of frames (0 = single frame)
	float FrameDuration = 0.25f; // Seconds per frame
	bool Looping = false;      // If true, this is a "state" animation; if false, a "one-shot"
};

// Runtime playback state for an animation clip.
// Separated from the data so the same clip can be used in multiple places.
class AnimationPlayback
{
public:
	AnimationPlayback();
	explicit AnimationPlayback(const AnimationClipData* clip);

	void SetClip(const AnimationClipData* clip);
	const AnimationClipData* GetClip() const { return Clip; }

	// Update playback state, returns true if animation finished this frame
	bool Advance(float deltaTime);
	
	// Apply current frame to sprite
	void ApplyToSprite(SpriteComponent& sprite) const;

	// State
	void Reset();
	bool IsFinished() const { return Finished; }
	bool IsPlaying() const { return Clip != nullptr && (!Finished || (Clip && Clip->Looping)); }
	int GetCurrentFrame() const { return CurrentFrame; }
	float GetNormalizedTime() const; // 0.0 to 1.0

private:
	const AnimationClipData* Clip = nullptr;
	int CurrentFrame = 0;
	float FrameTimer = 0.0f;
	bool Finished = false;
};

using AnimationClipPtr = std::unique_ptr<AnimationClipData>;
