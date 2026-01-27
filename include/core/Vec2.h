#pragma once
#include <cmath>

template<typename T>
struct Vector2 {
    T x;
    T y;

    Vector2() : x(0), y(0) {}
    Vector2(T x, T y) : x(x), y(y) {}

    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    Vector2 operator*(T scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    Vector2 operator/(T scalar) const {
        return Vector2(x / scalar, y / scalar);
    }

    Vector2& operator+=(const Vector2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    Vector2& operator-=(const Vector2& other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2& operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    bool operator==(const Vector2& other) const {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vector2& other) const {
        return !(*this == other);
    }

    T length() const {
        return std::sqrt(x * x + y * y);
    }

    Vector2 normalized() const {
        T len = length();
        if (len != 0) {
            return Vector2(x / len, y / len);
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

using Vec2 = Vector2<float>;
using Vec2i = Vector2<int>;
