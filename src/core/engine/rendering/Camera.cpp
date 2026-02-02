#include <core/rendering/Camera.h>
#include <core/rendering/RenderTypes.h>
#include <algorithm>
#include <cmath>

Camera::Camera()
	: Position(0, 0)
	, Rotation(0.0f)
	, Zoom(1.0f)
	, ViewSize(800, 600)
	, Target(0, 0)
	, HasTarget(false)
	, FollowSpeed(5.0f)
{
}

Camera::Camera(float _viewWidth, float _viewHeight)
	: Position(0, 0)
	, Rotation(0.0f)
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

void Camera::SetRotation(float _rotation)
{
	Rotation = _rotation;
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

CameraData Camera::GetCameraData() const
{
	CameraData data;
	data.Position = Position;
	data.Rotation = Rotation;
	data.ViewSize = ViewSize;
	data.Zoom = Zoom;
	return data;
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
	// Translate relative to camera position
	float relX = _worldPos.x - Position.x;
	float relY = _worldPos.y - Position.y;

	// Apply rotation (negative rotation to rotate world opposite to camera)
	float cosR = std::cos(-Rotation);
	float sinR = std::sin(-Rotation);
	float rotatedX = relX * cosR - relY * sinR;
	float rotatedY = relX * sinR + relY * cosR;

	// Apply zoom and center on screen
	return Vec2(
		rotatedX * Zoom + ViewSize.x * 0.5f,
		rotatedY * Zoom + ViewSize.y * 0.5f
	);
}

Vec2 Camera::ScreenToWorld(const Vec2& _screenPos) const
{
	// Reverse of WorldToScreen: un-center, un-zoom, un-rotate, un-translate
	float centeredX = _screenPos.x - ViewSize.x * 0.5f;
	float centeredY = _screenPos.y - ViewSize.y * 0.5f;

	// Un-zoom
	float unzoomedX = centeredX / Zoom;
	float unzoomedY = centeredY / Zoom;

	// Un-rotate (positive rotation since we're reversing)
	float cosR = std::cos(Rotation);
	float sinR = std::sin(Rotation);
	float unrotatedX = unzoomedX * cosR - unzoomedY * sinR;
	float unrotatedY = unzoomedX * sinR + unzoomedY * cosR;

	// Un-translate
	return Vec2(
		unrotatedX + Position.x,
		unrotatedY + Position.y
	);
}

float Camera::WorldToScreenX(float _worldX) const
{
	// Note: This simplified version ignores rotation for performance
	// Use WorldToScreen for accurate rotated coordinates
	return (_worldX - Position.x) * Zoom + ViewSize.x * 0.5f;
}

float Camera::WorldToScreenY(float _worldY) const
{
	// Note: This simplified version ignores rotation for performance
	// Use WorldToScreen for accurate rotated coordinates
	return (_worldY - Position.y) * Zoom + ViewSize.y * 0.5f;
}

float Camera::WorldToScreenScale(float _worldSize) const
{
	return _worldSize * Zoom;
}
