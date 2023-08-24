#pragma once
#include <cmath>
#include <numbers>

namespace Gng2D
{
struct V2d
{
    float x;
    float y;

    constexpr static float distance(V2d first, V2d second)
    {
        return std::sqrt(
                std::pow(first.x - second.x, 2) + std::pow(first.y - second.y, 2));
    }

    constexpr static float sqrDistance(V2d first, V2d second)
    {
        return  std::pow(first.x - second.x, 2) + std::pow(first.y - second.y, 2);
    }

    constexpr static V2d rot(float degrees, float length)
    {
        float rad = degrees * (std::numbers::pi / 180);
        float x = std::cos(rad) * length;
        float y = std::sin(rad) * length;
        return V2d{x, y};
    }

    constexpr static V2d normalize(V2d vec)
    {
        float len = std::sqrt((vec.x * vec.x) + (vec.y * vec.y));
        return {vec.x / len, vec.y / len};
    }

    constexpr static float dot(V2d lhs, V2d rhs)
    {
        return (lhs.x * rhs.x) + (lhs.y * rhs.y);
    }
    
    constexpr static float det(V2d lhs, V2d rhs)
    {
        return (lhs.x * rhs.y) - (lhs.y * rhs.x);
    }

    constexpr static float angle(V2d lhs, V2d rhs)
    {
        return std::atan2(det(lhs, rhs), dot(lhs, rhs))
            * 180 / std::numbers::pi;
    }

    constexpr V2d& operator+=(const V2d& rhs)
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }
};

constexpr bool operator==(const V2d& lhs, const V2d& rhs)
{
    constexpr float EPSILON = 0.001f;
    return  (lhs.x - rhs.x < EPSILON)
    and     (lhs.y - rhs.y < EPSILON);
}

constexpr V2d operator*(const float lhs, const V2d& rhs)
{
    return {lhs * rhs.x, lhs * rhs.y};
}

constexpr V2d operator*(const V2d& lhs, const float rhs)
{
    return {rhs * lhs.x, rhs * lhs.y};
}

constexpr V2d operator/(const V2d& lhs, const float rhs)
{
    return {lhs.x/rhs, lhs.y/rhs};
}

constexpr V2d operator+(const V2d& lhs, const V2d& rhs)
{
    return {lhs.x + rhs.x, lhs.y + rhs.y};
}

constexpr V2d operator-(const V2d& lhs, const V2d& rhs)
{
    return {lhs.x - rhs.x, lhs.y - rhs.y};
}
}

