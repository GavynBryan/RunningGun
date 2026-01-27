#include <core/Animation.h>



Animation::Animation(int _index, Vec2f _size, int _frames, bool _loop)
	:frameIndex(_index),
	frameSize(_size),
	numFrames(_frames),
	loop(_loop),
	finished(false),
	priority(false),
	currentFrame(0)
{
}

Animation::Animation(int _index, Vec2f _size, int _frames, bool _loop, bool _priority)
	:frameIndex(_index),
	frameSize(_size),
	numFrames(_frames),
	loop(_loop),
	finished(false),
	priority(_priority),
	currentFrame(0)
{
}

Animation::~Animation()
{
}

void Animation::update(Sprite& _sprite)
{
	_sprite.setTextureRect(Recti(
		static_cast<int>(currentFrame * frameSize.x),
		static_cast<int>(frameIndex * frameSize.y),
		static_cast<int>(frameSize.x),
		static_cast<int>(frameSize.y)
	));
	currentFrame++;
	if (currentFrame > numFrames) {
		if (loop)
			currentFrame = 0;
		else
			currentFrame = numFrames;
		finished = true;
	}
	else finished = false;
}
