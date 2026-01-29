#pragma once
#include <SDL3/SDL.h>
#include "Vec2.h"

enum class UIAnchor {
	TopLeft,
	TopCenter,
	TopRight,
	CenterLeft,
	Center,
	CenterRight,
	BottomLeft,
	BottomCenter,
	BottomRight
};

class UIElement
{
protected:
	Vec2		Position;
	Vec2		Size;
	UIAnchor	Anchor;
	bool		Visible;

	Vec2		CalculateScreenPosition(float _screenWidth, float _screenHeight) const;

public:
	UIElement();
	virtual ~UIElement();

	void			SetPosition(float _x, float _y);
	void			SetPosition(const Vec2& _pos);
	void			SetSize(float _width, float _height);
	void			SetAnchor(UIAnchor _anchor);
	void			SetVisible(bool _visible);

	Vec2			GetPosition() const { return Position; }
	Vec2			GetSize() const { return Size; }
	UIAnchor		GetAnchor() const { return Anchor; }
	bool			IsVisible() const { return Visible; }

	virtual void	Update(float _deltaTime);
	virtual void	Render(SDL_Renderer* _renderer, float _screenWidth, float _screenHeight) = 0;
};
