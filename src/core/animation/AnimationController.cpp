#include <animation/AnimationController.h>
#include <core/components/SpriteComponent.h>

AnimationController::AnimationController()
{
}

AnimationController::~AnimationController()
{
}

void AnimationController::AddClip(const std::string& name, AnimationClipData clip)
{
	OwnedClips[name] = std::move(clip);
	ClipRefs[name] = &OwnedClips[name];
}

void AnimationController::AddClip(const std::string& name, AnimationClipPtr clip)
{
	if (clip) {
		AddClip(name, std::move(*clip));
	}
}

void AnimationController::AddClipRef(const std::string& name, const AnimationClipData* clip)
{
	if (clip) {
		ClipRefs[name] = clip;
	}
}

const AnimationClipData* AnimationController::FindClip(std::string_view name) const
{
	auto it = ClipRefs.find(std::string(name));
	return (it != ClipRefs.end()) ? it->second : nullptr;
}

bool AnimationController::HasClip(std::string_view name) const
{
	return ClipRefs.find(std::string(name)) != ClipRefs.end();
}

void AnimationController::SetState(std::string_view stateName)
{
	if (CurrentStateName == stateName) {
		return;
	}

	const AnimationClipData* clip = FindClip(stateName);
	if (!clip) {
		return;
	}

	std::string_view oldState = CurrentStateName;
	TransitionToState(clip, stateName);
	OnStateChanged.Broadcast(oldState, CurrentStateName);
}

void AnimationController::PlayOneShot(std::string_view clipName, bool forceInterrupt)
{
	// If already playing a one-shot and not forcing interrupt, ignore
	if (OneShotActive && !forceInterrupt) {
		return;
	}

	const AnimationClipData* clip = FindClip(clipName);
	if (!clip) {
		return;
	}

	StartOneShot(clip, clipName);
}

void AnimationController::PlayAnimation(std::string_view name)
{
	const AnimationClipData* clip = FindClip(name);
	if (!clip) {
		return;
	}

	if (clip->Looping) {
		SetState(name);
	} else {
		PlayOneShot(name);
	}
}

void AnimationController::Update(float deltaTime, SpriteComponent& sprite)
{
	if (OneShotActive) {
		// Update one-shot animation
		bool finished = OneShotPlayback.Advance(deltaTime);
		OneShotPlayback.ApplyToSprite(sprite);

		if (finished) {
			std::string_view finishedName = CurrentOneShotName;
			FinishOneShot();
			OnOneShotFinished.Broadcast(finishedName);

			// Apply state animation frame immediately
			StatePlayback.ApplyToSprite(sprite);
		}
	} else {
		// Update state animation
		StatePlayback.Advance(deltaTime);
		StatePlayback.ApplyToSprite(sprite);
	}
}

void AnimationController::TransitionToState(const AnimationClipData* clip, std::string_view name)
{
	CurrentStateName = std::string(name);
	StatePlayback.SetClip(clip);
}

void AnimationController::StartOneShot(const AnimationClipData* clip, std::string_view name)
{
	CurrentOneShotName = std::string(name);
	OneShotPlayback.SetClip(clip);
	OneShotActive = true;
}

void AnimationController::FinishOneShot()
{
	OneShotActive = false;
	CurrentOneShotName.clear();
	OneShotPlayback.SetClip(nullptr);
}
