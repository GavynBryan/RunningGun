#include <core/UI/UIText.h>

UIText::UIText(SDL_Renderer* _renderer, TTF_Font* _font)
	: Font(_font)
	, Renderer(_renderer)
	, TextTexture(nullptr)
	, Text("")
	, Color{255, 255, 255, 255}
	, NeedsUpdate(false)
{
}

UIText::~UIText()
{
	if (TextTexture) {
		SDL_DestroyTexture(TextTexture);
	}
}

void UIText::RebuildTexture()
{
	if (TextTexture) {
		SDL_DestroyTexture(TextTexture);
		TextTexture = nullptr;
	}

	if (Text.empty() || !Font || !Renderer) {
		Size.x = 0;
		Size.y = 0;
		return;
	}

	SDL_Surface* _surface = TTF_RenderText_Solid(Font, Text.c_str(), 0, Color);
	if (_surface) {
		TextTexture = SDL_CreateTextureFromSurface(Renderer, _surface);
		Size.x = static_cast<float>(_surface->w);
		Size.y = static_cast<float>(_surface->h);
		SDL_DestroySurface(_surface);
	}
}

void UIText::SetText(const std::string& _text)
{
	if (Text != _text) {
		Text = _text;
		NeedsUpdate = true;
	}
}

void UIText::SetColor(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a)
{
	if (Color.r != _r || Color.g != _g || Color.b != _b || Color.a != _a) {
		Color = {_r, _g, _b, _a};
		NeedsUpdate = true;
	}
}

void UIText::Update(float _deltaTime)
{
	if (NeedsUpdate) {
		RebuildTexture();
		NeedsUpdate = false;
	}
}

void UIText::Render(SDL_Renderer* _renderer, float _screenWidth, float _screenHeight)
{
	if (!TextTexture || !Visible || Text.empty()) return;

	Vec2 _screenPos = CalculateScreenPosition(_screenWidth, _screenHeight);

	SDL_FRect _destRect = {
		_screenPos.x,
		_screenPos.y,
		Size.x,
		Size.y
	};

	SDL_RenderTexture(_renderer, TextTexture, nullptr, &_destRect);
}
