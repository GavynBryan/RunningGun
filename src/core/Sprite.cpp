#include <core/Sprite.h>

Sprite::Sprite()
    : Texture(nullptr)
    , DestRect{0, 0, 0, 0}
    , SrcRect(0, 0, 0, 0)
    , HasSrcRect(false)
{
}

Sprite::~Sprite()
{
    // Don't destroy texture - it's owned by ResourceHandler
}

void Sprite::SetTexture(SDL_Texture* _texture)
{
    Texture = _texture;
    if (_texture && !HasSrcRect) {
        float _width, _height;
        SDL_GetTextureSize(_texture, &_width, &_height);
        DestRect.w = _width;
        DestRect.h = _height;
    }
}

void Sprite::SetTextureRect(const Recti& _rect)
{
    SrcRect = _rect;
    HasSrcRect = true;
    DestRect.w = static_cast<float>(_rect.width);
    DestRect.h = static_cast<float>(_rect.height);
}

void Sprite::SetPosition(const Vec2& _pos)
{
    DestRect.x = _pos.x;
    DestRect.y = _pos.y;
}

void Sprite::SetPosition(float _x, float _y)
{
    DestRect.x = _x;
    DestRect.y = _y;
}

Rectf Sprite::GetGlobalBounds() const
{
    return Rectf(DestRect.x, DestRect.y, DestRect.w, DestRect.h);
}

void Sprite::Render(SDL_Renderer* _renderer)
{
    if (!Texture) return;

    if (HasSrcRect) {
        SDL_FRect _srcRect = {
            static_cast<float>(SrcRect.x),
            static_cast<float>(SrcRect.y),
            static_cast<float>(SrcRect.width),
            static_cast<float>(SrcRect.height)
        };
        SDL_RenderTexture(_renderer, Texture, &_srcRect, &DestRect);
    } else {
        SDL_RenderTexture(_renderer, Texture, nullptr, &DestRect);
    }
}
