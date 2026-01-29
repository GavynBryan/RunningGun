#pragma once
#include <SDL3/SDL.h>
#include <string>
#include <map>

#include <core/animation/Animation.h>

class AnimationStateMachine
{
protected:
	Animation*						CurrentAnimation;
	Animation*						PendingAnimation;
	std::map<std::string, AnimPtr>	AnimationMap;
	Uint64							LastAnimTime;
	bool							ForceRestart;

	void							TransitionTo(Animation* _next, Sprite& _sprite);
	bool							CanTransition(Animation* _next) const;
public:
	AnimationStateMachine();
	~AnimationStateMachine();

	void Update(Sprite& _sprite);
	void AddAnimation(const std::string& _name, AnimPtr _anim);
	bool IsNextPriority();
	void PlayAnimation(const std::string& _anim);
};
