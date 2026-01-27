#pragma once
#include <cmath>

template<typename T>
struct Vec2 {
    T x;
    T y;

    Vec2() : x(0), y(0) {}
    Vec2(T x, T y) : x(x), y(y) {}

    Vec2 operator+(const Vec2& other) const {
        return Vec2(x + other.x, y + other.y);
    }

    Vec2 operator-(const Vec2& other) const {
        return Vec2(x - other.x, y - other.y);
    }

    Vec2 operator*(T scalar) const {
        return Vec2(x * scalar, y * scalar);
    }

    Vec2 operator/(T scalar) const {
        return Vec2(x / scalar, y / scalar);
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vec2& operator-=(const Vec2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vec2& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec2& other) const {
        return !(*this == other);
    }

    T length() const {
        return std::sqrt(x * x + y * y);
    }

    Vec2 normalized() const {
        T len = length();
        if (len != 0) {
            return Vec2(x / len, y / len);
        }
        return *this;
    }

    void normalize() {
        T len = length();
        if (len != 0) {
            x /= len;
            y /= len;
        }
    }
};

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
