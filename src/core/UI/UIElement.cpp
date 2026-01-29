#include <core/UI/UIElement.h>

UIElement::UIElement()
	: Position(0, 0)
	, Size(0, 0)
	, Anchor(UIAnchor::TopLeft)
	, Visible(true)
{
}

UIElement::~UIElement()
{
}

Vec2 UIElement::CalculateScreenPosition(float _screenWidth, float _screenHeight) const
{
	Vec2 _screenPos = Position;

	switch (Anchor) {
	case UIAnchor::TopLeft:
		break;
	case UIAnchor::TopCenter:
		_screenPos.x += _screenWidth * 0.5f - Size.x * 0.5f;
		break;
	case UIAnchor::TopRight:
		_screenPos.x = _screenWidth - Position.x - Size.x;
		break;
	case UIAnchor::CenterLeft:
		_screenPos.y += _screenHeight * 0.5f - Size.y * 0.5f;
		break;
	case UIAnchor::Center:
		_screenPos.x += _screenWidth * 0.5f - Size.x * 0.5f;
		_screenPos.y += _screenHeight * 0.5f - Size.y * 0.5f;
		break;
	case UIAnchor::CenterRight:
		_screenPos.x = _screenWidth - Position.x - Size.x;
		_screenPos.y += _screenHeight * 0.5f - Size.y * 0.5f;
		break;
	case UIAnchor::BottomLeft:
		_screenPos.y = _screenHeight - Position.y - Size.y;
		break;
	case UIAnchor::BottomCenter:
		_screenPos.x += _screenWidth * 0.5f - Size.x * 0.5f;
		_screenPos.y = _screenHeight - Position.y - Size.y;
		break;
	case UIAnchor::BottomRight:
		_screenPos.x = _screenWidth - Position.x - Size.x;
		_screenPos.y = _screenHeight - Position.y - Size.y;
		break;
	}

	return _screenPos;
}

void UIElement::SetPosition(float _x, float _y)
{
	Position.x = _x;
	Position.y = _y;
}

void UIElement::SetPosition(const Vec2& _pos)
{
	Position = _pos;
}

void UIElement::SetSize(float _width, float _height)
{
	Size.x = _width;
	Size.y = _height;
}

void UIElement::SetAnchor(UIAnchor _anchor)
{
	Anchor = _anchor;
}

void UIElement::SetVisible(bool _visible)
{
	Visible = _visible;
}

void UIElement::Update(float _deltaTime)
{
}
