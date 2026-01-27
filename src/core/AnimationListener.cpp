#include <core/AnimationListener.h>
#include <cassert>
#include <iostream>

AnimationListener::AnimationListener()
	: CurrentAnimation(nullptr)
	, NextAnimation(nullptr)
	, LastAnimTime(SDL_GetTicks())
{
}


AnimationListener::~AnimationListener()
{
}

void AnimationListener::Update(Sprite& _sprite)
{
	if (CurrentAnimation) {
		//if it is a loop and there's another animation queued, then interrupt it
		if (CurrentAnimation->IsLoop() && NextAnimation) {
			CurrentAnimation = NextAnimation;
			NextAnimation = nullptr;
			CurrentAnimation->Update(_sprite);
			return;
		}
		Uint64 _currentTime = SDL_GetTicks();
		float _elapsedSeconds = (_currentTime - LastAnimTime) / 1000.0f;
		if (_elapsedSeconds >= 0.25f) {
			CurrentAnimation->Update(_sprite);
			//If it's NOT a loop and it HAS finished, then play the next queued anim
			LastAnimTime = _currentTime;
		}
		if (!CurrentAnimation->IsLoop() && NextAnimation) {
			if (NextAnimation->IsPriority() || CurrentAnimation->IsFinished()) {
				CurrentAnimation = NextAnimation;
				NextAnimation = nullptr;
				LastAnimTime = SDL_GetTicks();
			}
		}


	//There is no current animation, but there is one queued up
	} else if (NextAnimation) {
		CurrentAnimation = NextAnimation;
		NextAnimation = nullptr;
		LastAnimTime = SDL_GetTicks();
	}
}

void AnimationListener::AddAnimation(const std::string& _name, AnimPtr _anim)
{
	AnimationMap.insert(std::make_pair(_name, std::move(_anim)));
}

//is the next animation a priority animation?
bool AnimationListener::IsNextPriority()
{
	if (NextAnimation && NextAnimation->IsPriority())
		return true;
	return false;
}

void AnimationListener::PlayAnimation(const std::string& _anim)
{
	auto _found = AnimationMap.find(_anim);
	assert(_found != AnimationMap.end());

	auto _animTarget = _found->second.get();
	//if (isNextPriority() && !anim->isPriority()) return;
	if (_animTarget != CurrentAnimation) {
		NextAnimation = _animTarget;
	}

}
