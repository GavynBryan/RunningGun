#include <core/Animation.h>



Animation::Animation(int _index, Vec2 _size, int _frames, bool _loop)
	:FrameIndex(_index),
	FrameSize(_size),
	NumFrames(_frames),
	Loop(_loop),
	Finished(false),
	Priority(false),
	CurrentFrame(0)
{
}

Animation::Animation(int _index, Vec2 _size, int _frames, bool _loop, bool _priority)
	:FrameIndex(_index),
	FrameSize(_size),
	NumFrames(_frames),
	Loop(_loop),
	Finished(false),
	Priority(_priority),
	CurrentFrame(0)
{
}

Animation::~Animation()
{
}

void Animation::Update(Sprite& _sprite)
{
	_sprite.SetTextureRect(Recti(
		static_cast<int>(CurrentFrame * FrameSize.x),
		static_cast<int>(FrameIndex * FrameSize.y),
		static_cast<int>(FrameSize.x),
		static_cast<int>(FrameSize.y)
	));
	CurrentFrame++;
	if (CurrentFrame > NumFrames) {
		if (Loop)
			CurrentFrame = 0;
		else
			CurrentFrame = NumFrames;
		Finished = true;
	}
	else Finished = false;
}

void Animation::Reset()
{
	CurrentFrame = 0;
	Finished = false;
}
