#include <core/animation/AnimationStateMachine.h>
#include <cassert>

AnimationStateMachine::AnimationStateMachine()
	: CurrentAnimation(nullptr)
	, PendingAnimation(nullptr)
	, LastAnimTime(SDL_GetTicks())
	, ForceRestart(false)
{
}


AnimationStateMachine::~AnimationStateMachine()
{
}

void AnimationStateMachine::Update(Sprite& _sprite)
{
	if (PendingAnimation && (ForceRestart || CanTransition(PendingAnimation))) {
		TransitionTo(PendingAnimation, _sprite);
	}

	if (CurrentAnimation) {
		Uint64 _currentTime = SDL_GetTicks();
		float _elapsedSeconds = (_currentTime - LastAnimTime) / 1000.0f;
		if (_elapsedSeconds >= 0.25f) {
			CurrentAnimation->Update(_sprite);
			LastAnimTime = _currentTime;
		}
	} else if (PendingAnimation) {
		TransitionTo(PendingAnimation, _sprite);
	}
}

void AnimationStateMachine::AddAnimation(const std::string& _name, AnimPtr _anim)
{
	AnimationMap.insert(std::make_pair(_name, std::move(_anim)));
}

//is the next animation a priority animation?
bool AnimationStateMachine::IsNextPriority()
{
	if (PendingAnimation && PendingAnimation->IsPriority())
		return true;
	return false;
}

void AnimationStateMachine::PlayAnimation(const std::string& _anim)
{
	auto _found = AnimationMap.find(_anim);
	assert(_found != AnimationMap.end());

	auto _animTarget = _found->second.get();
	if (_animTarget == CurrentAnimation) {
		if (!CurrentAnimation->IsLoop() && CurrentAnimation->IsFinished()) {
			PendingAnimation = _animTarget;
			ForceRestart = true;
		}
		return;
	}
	if (CurrentAnimation && CurrentAnimation->IsPriority() && !CurrentAnimation->IsFinished() &&
		!_animTarget->IsPriority()) {
		return;
	}
	PendingAnimation = _animTarget;
	ForceRestart = false;
}

void AnimationStateMachine::TransitionTo(Animation* _next, Sprite& _sprite)
{
	if (!_next) {
		return;
	}
	CurrentAnimation = _next;
	PendingAnimation = nullptr;
	ForceRestart = false;
	CurrentAnimation->Reset();
	CurrentAnimation->Update(_sprite);
	LastAnimTime = SDL_GetTicks();
}

bool AnimationStateMachine::CanTransition(Animation* _next) const
{
	if (!_next) {
		return false;
	}
	if (!CurrentAnimation) {
		return true;
	}
	if (CurrentAnimation->IsLoop()) {
		return true;
	}
	if (_next->IsPriority()) {
		return true;
	}
	if (CurrentAnimation->IsPriority() && !CurrentAnimation->IsFinished()) {
		return false;
	}
	return CurrentAnimation->IsFinished();
}
