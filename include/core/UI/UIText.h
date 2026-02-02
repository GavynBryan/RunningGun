#pragma once
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>
#include <core/UI/UIElement.h>

class UIText : public UIElement
{
private:
	TTF_Font*		Font;
	SDL_Renderer*	Renderer;
	SDL_Texture*	TextTexture;
	std::string		Text;
	SDL_Color		Color;
	bool			NeedsUpdate;

	void			RebuildTexture();

public:
	UIText(SDL_Renderer* _renderer, TTF_Font* _font);
	~UIText() override;

	void		SetText(const std::string& _text);
	void		SetColor(Uint8 _r, Uint8 _g, Uint8 _b, Uint8 _a = 255);

	const std::string&	GetText() const { return Text; }

	void		Update(float _deltaTime) override;
	void		Render(SDL_Renderer* _renderer, float _screenWidth, float _screenHeight) override;
};
