#pragma once
#include "Vec2.h"

template<typename T>
struct Rect {
    T x;
    T y;
    T width;
    T height;

    Rect() : x(0), y(0), width(0), height(0) {}
    Rect(T x, T y, T width, T height) : x(x), y(y), width(width), height(height) {}

    T left() const { return x; }
    T top() const { return y; }
    T right() const { return x + width; }
    T bottom() const { return y + height; }

    Vec2<T> position() const { return Vec2<T>(x, y); }
    Vec2<T> size() const { return Vec2<T>(width, height); }

    bool contains(T px, T py) const {
        return px >= x && px < x + width && py >= y && py < y + height;
    }

    bool contains(const Vec2<T>& point) const {
        return contains(point.x, point.y);
    }

    bool intersects(const Rect& other) const {
        return !(right() <= other.left() ||
                 left() >= other.right() ||
                 bottom() <= other.top() ||
                 top() >= other.bottom());
    }
};

using Rectf = Rect<float>;
using Recti = Rect<int>;
