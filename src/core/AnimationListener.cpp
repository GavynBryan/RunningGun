#include <core/AnimationListener.h>
#include <cassert>
#include <iostream>

AnimationListener::AnimationListener()
	: mCurrentAnimation(nullptr)
	, mNextAnimation(nullptr)
	, lastAnimTime(SDL_GetTicks())
{
}


AnimationListener::~AnimationListener()
{
}

void AnimationListener::update(Sprite& _sprite)
{
	if (mCurrentAnimation) {
		//if it is a loop and there's another animation queued, then interrupt it
		if (mCurrentAnimation->isLoop() && mNextAnimation) {
			mCurrentAnimation = mNextAnimation;
			mNextAnimation = nullptr;
			mCurrentAnimation->update(_sprite);
			return;
		}
		Uint64 currentTime = SDL_GetTicks();
		float elapsedSeconds = (currentTime - lastAnimTime) / 1000.0f;
		if (elapsedSeconds >= 0.25f) {
			mCurrentAnimation->update(_sprite);
			//If it's NOT a loop and it HAS finished, then play the next queued anim
			lastAnimTime = currentTime;
		}
		if (!mCurrentAnimation->isLoop() && mNextAnimation) {
			if (mNextAnimation->isPriority() || mCurrentAnimation->isFinished()) {
				mCurrentAnimation = mNextAnimation;
				mNextAnimation = nullptr;
				lastAnimTime = SDL_GetTicks();
			}
		}


	//There is no current animation, but there is one queued up
	} else if (mNextAnimation) {
		mCurrentAnimation = mNextAnimation;
		mNextAnimation = nullptr;
		lastAnimTime = SDL_GetTicks();
	}
}

void AnimationListener::addAnimation(const std::string& _name, AnimPtr _anim)
{
	mAnimationMap.insert(std::make_pair(_name, std::move(_anim)));
}

//is the next animation a priority animation?
bool AnimationListener::isNextPriority()
{
	if (mNextAnimation && mNextAnimation->isPriority())
		return true;
	return false;
}

void AnimationListener::playAnimation(const std::string& _anim)
{
	auto found = mAnimationMap.find(_anim);
	assert(found != mAnimationMap.end());

	auto anim = found->second.get();
	//if (isNextPriority() && !anim->isPriority()) return;
	if (anim != mCurrentAnimation) {
		mNextAnimation = anim;
	}

}
