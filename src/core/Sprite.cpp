#include <core/Sprite.h>

Sprite::Sprite()
    : mTexture(nullptr)
    , mDestRect{0, 0, 0, 0}
    , mSrcRect(0, 0, 0, 0)
    , mHasSrcRect(false)
{
}

Sprite::~Sprite()
{
    // Don't destroy texture - it's owned by ResourceHandler
}

void Sprite::setTexture(SDL_Texture* texture)
{
    mTexture = texture;
    if (texture && !mHasSrcRect) {
        float w, h;
        SDL_GetTextureSize(texture, &w, &h);
        mDestRect.w = w;
        mDestRect.h = h;
    }
}

void Sprite::setTextureRect(const Recti& rect)
{
    mSrcRect = rect;
    mHasSrcRect = true;
    mDestRect.w = static_cast<float>(rect.width);
    mDestRect.h = static_cast<float>(rect.height);
}

void Sprite::setPosition(const Vec2& pos)
{
    mDestRect.x = pos.x;
    mDestRect.y = pos.y;
}

void Sprite::setPosition(float x, float y)
{
    mDestRect.x = x;
    mDestRect.y = y;
}

Rectf Sprite::getGlobalBounds() const
{
    return Rectf(mDestRect.x, mDestRect.y, mDestRect.w, mDestRect.h);
}

void Sprite::render(SDL_Renderer* renderer)
{
    if (!mTexture) return;

    if (mHasSrcRect) {
        SDL_FRect srcRect = {
            static_cast<float>(mSrcRect.x),
            static_cast<float>(mSrcRect.y),
            static_cast<float>(mSrcRect.width),
            static_cast<float>(mSrcRect.height)
        };
        SDL_RenderTexture(renderer, mTexture, &srcRect, &mDestRect);
    } else {
        SDL_RenderTexture(renderer, mTexture, nullptr, &mDestRect);
    }
}
