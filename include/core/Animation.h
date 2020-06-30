#pragma once
#include <memory>
#include <SFML/Graphics/Sprite.hpp>
class Animation
{
private:
	sf::Vector2f	frameSize;
	int				numFrames;
	int				frameIndex;

	int				currentFrame;

	bool			loop;
	bool			priority;
	bool			finished;

public:
	Animation(int _index, sf::Vector2f _size, int _frames, bool _loop);
	Animation(int _index, sf::Vector2f _size, int _frames, bool _loop, bool _priority);
	~Animation();
	
	void update(sf::Sprite& _sprite);

	bool isLoop() { return loop; }
	bool isFinished() { return finished; }
	bool isPriority() { return priority; }
};

typedef std::unique_ptr<Animation> AnimPtr;