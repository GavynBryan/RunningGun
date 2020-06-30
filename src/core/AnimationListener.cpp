#include <core/AnimationListener.h>
#include <cassert>
#include <iostream>

AnimationListener::AnimationListener()
{
	animClock.restart();
}


AnimationListener::~AnimationListener()
{
}

void AnimationListener::update(sf::Sprite& _sprite)
{
	if (mCurrentAnimation) {
		//if it is a loop and there's another animation queued, then interrupt it
		if (mCurrentAnimation->isLoop() && mNextAnimation) {
			mCurrentAnimation = mNextAnimation;
			mNextAnimation = nullptr;
			mCurrentAnimation->update(_sprite);
			return;
		}
		if (animClock.getElapsedTime().asSeconds() >= 0.25f) {
			mCurrentAnimation->update(_sprite);
			//If it's NOT a loop and it HAS finished, then play the next queued anim
			animClock.restart();
		}
		if (!mCurrentAnimation->isLoop() && mNextAnimation) {
			if (mNextAnimation->isPriority() || mCurrentAnimation->isFinished()) {
				mCurrentAnimation = mNextAnimation;
				mNextAnimation = nullptr;
				animClock.restart();
			}
		}

		
	//There is no current animation, but there is one queued up
	} else if (mNextAnimation) {
		mCurrentAnimation = mNextAnimation;
		mNextAnimation = nullptr;
		animClock.restart();
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