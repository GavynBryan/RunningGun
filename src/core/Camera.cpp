#include <core/Camera.h>
#include <algorithm>

Camera::Camera()
	: Position(0, 0)
	, Zoom(1.0f)
	, ViewSize(800, 600)
	, Target(0, 0)
	, HasTarget(false)
	, FollowSpeed(5.0f)
{
}

Camera::Camera(float _viewWidth, float _viewHeight)
	: Position(0, 0)
	, Zoom(1.0f)
	, ViewSize(_viewWidth, _viewHeight)
	, Target(0, 0)
	, HasTarget(false)
	, FollowSpeed(5.0f)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float _x, float _y)
{
	Position.x = _x;
	Position.y = _y;
}

void Camera::SetPosition(const Vec2& _pos)
{
	Position = _pos;
}

void Camera::SetZoom(float _zoom)
{
	Zoom = std::max(0.1f, _zoom);
}

void Camera::SetViewSize(float _width, float _height)
{
	ViewSize.x = _width;
	ViewSize.y = _height;
}

void Camera::SetTarget(const Vec2& _target)
{
	Target = _target;
	HasTarget = true;
}

void Camera::SetFollowSpeed(float _speed)
{
	FollowSpeed = _speed;
}

void Camera::ClearTarget()
{
	HasTarget = false;
}

void Camera::Update(float _deltaTime)
{
	if (HasTarget) {
		Vec2 _targetCameraPos = Target - (ViewSize * 0.5f) / Zoom;
		Vec2 _diff = _targetCameraPos - Position;
		Position += _diff * (FollowSpeed * _deltaTime);
	}
}

Vec2 Camera::WorldToScreen(const Vec2& _worldPos) const
{
	return (_worldPos - Position) * Zoom;
}

Vec2 Camera::ScreenToWorld(const Vec2& _screenPos) const
{
	return (_screenPos / Zoom) + Position;
}

float Camera::WorldToScreenX(float _worldX) const
{
	return (_worldX - Position.x) * Zoom;
}

float Camera::WorldToScreenY(float _worldY) const
{
	return (_worldY - Position.y) * Zoom;
}

float Camera::WorldToScreenScale(float _worldSize) const
{
	return _worldSize * Zoom;
}
