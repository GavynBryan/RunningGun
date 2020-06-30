#include <core/Animation.h>



Animation::Animation(int _index, sf::Vector2f _size, int _frames, bool _loop)
	:frameIndex(_index),
	frameSize(_size),
	numFrames(_frames),
	loop(_loop),
	finished(false),
	priority(false),
	currentFrame(0)
{
}

Animation::Animation(int _index, sf::Vector2f _size, int _frames, bool _loop, bool _priority)
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

//Returns whether or not the animation is finished
void Animation::update(sf::Sprite& _sprite)
{
	_sprite.setTextureRect(sf::IntRect(currentFrame * frameSize.x,
						frameIndex * frameSize.y, frameSize.x, frameSize.y));
	currentFrame ++;
	if (currentFrame > numFrames) {
		if (loop)
			currentFrame = 0;
		else
			currentFrame = numFrames;
		finished = true;
	}
	else finished = false;
}
