#pragma once
#include <SDL3/SDL.h>
#include "Vec2.h"
#include "Rect.h"

class Sprite {
private:
    SDL_Texture*    mTexture;
    SDL_FRect       mDestRect;
    Recti           mSrcRect;
    bool            mHasSrcRect;

public:
    Sprite();
    ~Sprite();

    void setTexture(SDL_Texture* texture);
    void setTextureRect(const Recti& rect);
    void setPosition(const Vec2f& pos);
    void setPosition(float x, float y);

    SDL_Texture* getTexture() const { return mTexture; }
    const SDL_FRect& getDestRect() const { return mDestRect; }
    Rectf getGlobalBounds() const;

    void render(SDL_Renderer* renderer);
};
