#pragma once
#include <core/Vec2.h>

template<typename T>
struct Rect {
    T x;
    T y;
    T width;
    T height;

    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(T _x, T _y, T _width, T _height) : x(_x), y(_y), width(_width), height(_height) {}

    T Left() const { return x; }
    T Top() const { return y; }
    T Right() const { return x + width; }
    T Bottom() const { return y + height; }

    Vec2<T> Position() const { return Vec2<T>(x, y); }
    Vec2<T> Size() const { return Vec2<T>(width, height); }

    bool Contains(T _x, T _y) const {
        return _x >= x && _x < x + width && _y >= y && _y < y + height;
    }

    bool Contains(const Vec2<T>& _point) const {
        return Contains(_point.x, _point.y);
    }

    bool Intersects(const Rect& _other) const {
        return !(Right() <= _other.Left() ||
                 Left() >= _other.Right() ||
                 Bottom() <= _other.Top() ||
                 Top() >= _other.Bottom());
    }
};

using Rectf = Rect<float>;
using Recti = Rect<int>;
