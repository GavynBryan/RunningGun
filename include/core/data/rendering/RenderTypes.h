#pragma once

#include <core/math/Rect.h>
#include <core/math/Vec2.h>
#include <cstdint>
#include <vector>

// Color for rendering operations
struct Color
{
	uint8_t r = 255;
	uint8_t g = 255;
	uint8_t b = 255;
	uint8_t a = 255;

	Color() = default;
	Color(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _a = 255)
		: r(_r), g(_g), b(_b), a(_a) {}

	static Color White() { return Color(255, 255, 255); }
	static Color Black() { return Color(0, 0, 0); }
	static Color Red() { return Color(255, 0, 0); }
	static Color Green() { return Color(0, 255, 0); }
	static Color Blue() { return Color(0, 0, 255); }
	static Color Transparent() { return Color(0, 0, 0, 0); }
};

// Parameters for sprite rendering
struct SpriteRenderParams
{
	Rectf SourceRect;
	Rectf DestRect;
	bool FlipX = false;
	bool FlipY = false;
	float Rotation = 0.0f;
	Vec2 Origin = Vec2(0.0f, 0.0f);
	Color Tint = Color::White();
};

// Camera state for view transformation
struct CameraData
{
	Vec2 Position;
	float Rotation = 0.0f;
	Vec2 ViewSize;
	float Zoom = 1.0f;
};

// Raw pixel data for texture upload
struct TextureData
{
	std::vector<uint8_t> Pixels;
	int Width = 0;
	int Height = 0;
	int Channels = 4; // RGBA
};
