#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <map>

#include "Animation.h"

class AnimationListener
{
protected:
	Animation*						mCurrentAnimation;
	Animation*						mNextAnimation;
	std::map<std::string, AnimPtr>	mAnimationMap;
	Uint64							lastAnimTime;
public:
	AnimationListener();
	~AnimationListener();

	void update(Sprite& _sprite);
	void addAnimation(const std::string& _name, AnimPtr _anim);
	bool isNextPriority();
	void playAnimation(const std::string& _anim);
};
