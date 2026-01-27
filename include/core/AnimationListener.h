#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <map>

#include "Animation.h"

class AnimationListener
{
protected:
	Animation*						CurrentAnimation;
	Animation*						NextAnimation;
	std::map<std::string, AnimPtr>	AnimationMap;
	Uint64							LastAnimTime;
public:
	AnimationListener();
	~AnimationListener();

	void Update(Sprite& _sprite);
	void AddAnimation(const std::string& _name, AnimPtr _anim);
	bool IsNextPriority();
	void PlayAnimation(const std::string& _anim);
};
