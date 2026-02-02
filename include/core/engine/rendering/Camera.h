#pragma once

#include <core/math/Vec2.h>

// Forward declaration to avoid circular includes
struct CameraData;

class Camera
{
private:
	Vec2	Position;
	float	Rotation;
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
	void	SetRotation(float _rotation);
	void	SetZoom(float _zoom);
	void	SetViewSize(float _width, float _height);
	void	SetTarget(const Vec2& _target);
	void	SetFollowSpeed(float _speed);
	void	ClearTarget();

	void	Update(float _deltaTime);

	Vec2	GetPosition() const { return Position; }
	float	GetRotation() const { return Rotation; }
	float	GetZoom() const { return Zoom; }
	Vec2	GetViewSize() const { return ViewSize; }

	// Get camera data for graphics API
	CameraData GetCameraData() const;

	// Coordinate transformations (rotation-aware)
	Vec2	WorldToScreen(const Vec2& _worldPos) const;
	Vec2	ScreenToWorld(const Vec2& _screenPos) const;

	// Simplified single-axis transforms (ignores rotation for performance)
	float	WorldToScreenX(float _worldX) const;
	float	WorldToScreenY(float _worldY) const;
	float	WorldToScreenScale(float _worldSize) const;
};
