#pragma once
#include <string>
#include <map>
#include <SFML/System/Clock.hpp>

#include "Animation.h"

class AnimationListener
{
protected:
	Animation*						mCurrentAnimation;
	Animation*						mNextAnimation;
	std::map<std::string, AnimPtr>	mAnimationMap;
	sf::Clock						animClock;
public:
	AnimationListener();
	~AnimationListener();

	void update(sf::Sprite& _sprite);
	void addAnimation(const std::string& _name, AnimPtr _anim);
	bool isNextPriority();
	void playAnimation(const std::string& _anim);
};

