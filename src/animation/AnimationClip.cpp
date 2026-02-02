#include <animation/AnimationClip.h>
#include <core/components/SpriteComponent.h>

AnimationPlayback::AnimationPlayback()
	: Clip(nullptr)
	, CurrentFrame(0)
	, FrameTimer(0.0f)
	, Finished(false)
{
}

AnimationPlayback::AnimationPlayback(const AnimationClipData* clip)
	: Clip(clip)
	, CurrentFrame(0)
	, FrameTimer(0.0f)
	, Finished(false)
{
}

void AnimationPlayback::SetClip(const AnimationClipData* clip)
{
	Clip = clip;
	Reset();
}

bool AnimationPlayback::Advance(float deltaTime)
{
	if (!Clip || Finished) {
		return false;
	}

	// Single frame animation - no advancement needed
	if (Clip->FrameCount <= 1) {
		Finished = !Clip->Looping;
		return Finished;
	}

	FrameTimer += deltaTime;

	bool completedCycle = false;

	while (FrameTimer >= Clip->FrameDuration) {
		FrameTimer -= Clip->FrameDuration;
		CurrentFrame++;

		if (CurrentFrame >= Clip->FrameCount) {
			if (Clip->Looping) {
				CurrentFrame = 0;
			} else {
				CurrentFrame = Clip->FrameCount - 1;
				Finished = true;
				completedCycle = true;
				break;
			}
			completedCycle = true;
		}
	}

	return completedCycle && !Clip->Looping;
}

void AnimationPlayback::ApplyToSprite(SpriteComponent& sprite) const
{
	if (!Clip) {
		return;
	}

	// Calculate source rectangle based on current frame
	float frameX = static_cast<float>(CurrentFrame) * Clip->FrameSize.x;
	float frameY = static_cast<float>(Clip->RowIndex) * Clip->FrameSize.y;

	sprite.SetSourceRect(Rectf(frameX, frameY, Clip->FrameSize.x, Clip->FrameSize.y));
}

void AnimationPlayback::Reset()
{
	CurrentFrame = 0;
	FrameTimer = 0.0f;
	Finished = false;
}

float AnimationPlayback::GetNormalizedTime() const
{
	if (!Clip || Clip->FrameCount <= 1) {
		return Finished ? 1.0f : 0.0f;
	}

	float totalDuration = Clip->FrameDuration * static_cast<float>(Clip->FrameCount);
	float currentTime = Clip->FrameDuration * static_cast<float>(CurrentFrame) + FrameTimer;

	return currentTime / totalDuration;
}
