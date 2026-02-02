#include <core/UI/UIHealthBar.h>
#include <algorithm>

UIHealthBar::UIHealthBar(SDL_Texture* _heartTexture, int _maxHearts)
	: HeartTexture(_heartTexture)
	, MaxHearts(_maxHearts)
	, CurrentHearts(_maxHearts)
	, HeartWidth(32.0f)
	, HeartHeight(32.0f)
	, HeartSpacing(0.0f)
	, HealthGetter(nullptr)
{
	Size.x = (_maxHearts * HeartWidth) + ((_maxHearts - 1) * HeartSpacing);
	Size.y = HeartHeight;
}

UIHealthBar::~UIHealthBar()
{
}

void UIHealthBar::SetHeartSize(float _width, float _height)
{
	HeartWidth = _width;
	HeartHeight = _height;
	Size.x = (MaxHearts * HeartWidth) + ((MaxHearts - 1) * HeartSpacing);
	Size.y = HeartHeight;
}

void UIHealthBar::SetHeartSpacing(float _spacing)
{
	HeartSpacing = _spacing;
	Size.x = (MaxHearts * HeartWidth) + ((MaxHearts - 1) * HeartSpacing);
}

void UIHealthBar::SetCurrentHearts(int _hearts)
{
	CurrentHearts = std::clamp(_hearts, 0, MaxHearts);
}

void UIHealthBar::SetHealthGetter(std::function<int()> _getter)
{
	HealthGetter = _getter;
}

void UIHealthBar::Update(float _deltaTime)
{
	if (HealthGetter) {
		CurrentHearts = std::clamp(HealthGetter(), 0, MaxHearts);
	}
}

void UIHealthBar::Render(SDL_Renderer* _renderer, float _screenWidth, float _screenHeight)
{
	if (!HeartTexture || !Visible) return;

	Vec2 _screenPos = CalculateScreenPosition(_screenWidth, _screenHeight);

	for (int _i = 0; _i < CurrentHearts; _i++) {
		SDL_FRect _destRect = {
			_screenPos.x + (_i * (HeartWidth + HeartSpacing)),
			_screenPos.y,
			HeartWidth,
			HeartHeight
		};
		SDL_RenderTexture(_renderer, HeartTexture, nullptr, &_destRect);
	}
}
