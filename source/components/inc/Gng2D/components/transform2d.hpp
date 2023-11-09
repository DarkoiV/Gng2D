#pragma once
#include <compare>
#include <cstdint>

namespace Gng2D {

struct Transform2d
{
    float x;
    float y;

    struct Layer
    {
        int8_t main;
        int8_t sub;
    } layer;
};

namespace detail {
struct Position
{
    float x;
    float y;

    struct Layer
    {
        int16_t main;
        int16_t sub;

        std::strong_ordering operator<=>(const Layer&) const = default;
    } layer;
};
} // namespace detail
} // namespace Gng2D

