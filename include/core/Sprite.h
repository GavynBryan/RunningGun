#pragma once
#include <SDL3/SDL.h>
#include "Vec2.h"
#include "Rect.h"

class Camera;

class Sprite {
private:
    SDL_Texture*    Texture;
    SDL_FRect       DestRect;
    Recti           SrcRect;
    bool            HasSrcRect;

public:
    Sprite();
    ~Sprite();

    void SetTexture(SDL_Texture* _texture);
    void SetTextureRect(const Recti& _rect);
    void SetPosition(const Vec2& _pos);
    void SetPosition(float _x, float _y);

    SDL_Texture* GetTexture() const { return Texture; }
    const SDL_FRect& GetDestRect() const { return DestRect; }
    Rectf GetGlobalBounds() const;

    void Render(SDL_Renderer* _renderer, Camera* _camera = nullptr);
};
