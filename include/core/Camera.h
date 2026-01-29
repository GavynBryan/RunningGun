#pragma once
#include "Vec2.h"

class Camera
{
private:
	Vec2	Position;
	float	Zoom;
	Vec2	ViewSize;
	Vec2	Target;
	bool	HasTarget;
	float	FollowSpeed;

public:
	Camera();
	Camera(float _viewWidth, float _viewHeight);
	~Camera();

	void	SetPosition(float _x, float _y);
	void	SetPosition(const Vec2& _pos);
	void	SetZoom(float _zoom);
	void	SetViewSize(float _width, float _height);
	void	SetTarget(const Vec2& _target);
	void	SetFollowSpeed(float _speed);
	void	ClearTarget();

	void	Update(float _deltaTime);

	Vec2	GetPosition() const { return Position; }
	float	GetZoom() const { return Zoom; }
	Vec2	GetViewSize() const { return ViewSize; }

	Vec2	WorldToScreen(const Vec2& _worldPos) const;
	Vec2	ScreenToWorld(const Vec2& _screenPos) const;

	float	WorldToScreenX(float _worldX) const;
	float	WorldToScreenY(float _worldY) const;
	float	WorldToScreenScale(float _worldSize) const;
};
