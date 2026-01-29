#pragma once
#include "UIElement.h"
#include <functional>

class UIHealthBar : public UIElement
{
private:
	SDL_Texture*				HeartTexture;
	int							MaxHearts;
	int							CurrentHearts;
	float						HeartWidth;
	float						HeartHeight;
	float						HeartSpacing;
	std::function<int()>		HealthGetter;

public:
	UIHealthBar(SDL_Texture* _heartTexture, int _maxHearts = 5);
	~UIHealthBar() override;

	void		SetHeartSize(float _width, float _height);
	void		SetHeartSpacing(float _spacing);
	void		SetCurrentHearts(int _hearts);
	void		SetHealthGetter(std::function<int()> _getter);

	int			GetCurrentHearts() const { return CurrentHearts; }
	int			GetMaxHearts() const { return MaxHearts; }

	void		Update(float _deltaTime) override;
	void		Render(SDL_Renderer* _renderer, float _screenWidth, float _screenHeight) override;
};
