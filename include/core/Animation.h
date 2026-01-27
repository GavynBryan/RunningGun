#pragma once
#include <memory>
#include "Vec2.h"
#include "Sprite.h"

class Animation
{
private:
	Vec2f			frameSize;
	int				numFrames;
	int				frameIndex;

	int				currentFrame;

	bool			loop;
	bool			priority;
	bool			finished;

public:
	Animation(int _index, Vec2f _size, int _frames, bool _loop);
	Animation(int _index, Vec2f _size, int _frames, bool _loop, bool _priority);
	~Animation();

	void update(Sprite& _sprite);

	bool isLoop() { return loop; }
	bool isFinished() { return finished; }
	bool isPriority() { return priority; }
};

typedef std::unique_ptr<Animation> AnimPtr;
