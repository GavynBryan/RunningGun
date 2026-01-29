#pragma once
#include <memory>
#include <core/Vec2.h>
#include <core/Sprite.h>

class Animation
{
private:
	Vec2			FrameSize;
	int				NumFrames;
	int				FrameIndex;

	int				CurrentFrame;

	bool			Loop;
	bool			Priority;
	bool			Finished;

public:
	Animation(int _index, Vec2 _size, int _frames, bool _loop);
	Animation(int _index, Vec2 _size, int _frames, bool _loop, bool _priority);
	~Animation();

	void Update(Sprite& _sprite);
	void Reset();

	bool IsLoop() { return Loop; }
	bool IsFinished() { return Finished; }
	bool IsPriority() { return Priority; }
};

typedef std::unique_ptr<Animation> AnimPtr;
