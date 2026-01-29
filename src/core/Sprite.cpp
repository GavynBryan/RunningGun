#include <core/Sprite.h>
#include <core/Camera.h>

Sprite::Sprite()
    : Texture(nullptr)
    , DestRect{0, 0, 0, 0}
    , SrcRect(0, 0, 0, 0)
    , HasSrcRect(false)
    , FlipMode(SDL_FLIP_NONE)
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

void Sprite::SetFlipX(bool _flip)
{
    FlipMode = _flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
}

Rectf Sprite::GetGlobalBounds() const
{
    return Rectf(DestRect.x, DestRect.y, DestRect.w, DestRect.h);
}

void Sprite::Render(SDL_Renderer* _renderer, Camera* _camera)
{
    if (!Texture) return;

    SDL_FRect _destRect = DestRect;
    if (_camera) {
        Vec2 _screenPos = _camera->WorldToScreen(Vec2(DestRect.x, DestRect.y));
        float _zoom = _camera->GetZoom();
        _destRect.x = _screenPos.x;
        _destRect.y = _screenPos.y;
        _destRect.w = DestRect.w * _zoom;
        _destRect.h = DestRect.h * _zoom;
    }

    SDL_FRect _srcRect = {
        static_cast<float>(SrcRect.x),
        static_cast<float>(SrcRect.y),
        static_cast<float>(SrcRect.width),
        static_cast<float>(SrcRect.height)
    };
    const SDL_FRect* _srcPtr = HasSrcRect ? &_srcRect : nullptr;

    if (FlipMode != SDL_FLIP_NONE) {
        SDL_RenderTextureRotated(_renderer, Texture, _srcPtr, &_destRect, 0.0, nullptr, FlipMode);
        return;
    }

    SDL_RenderTexture(_renderer, Texture, _srcPtr, &_destRect);
}
