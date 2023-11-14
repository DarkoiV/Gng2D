#pragma once
#include "Gng2D/commons/component_args.hpp"
#include <compare>
#include <cstdint>

namespace Gng2D {
struct ArgsVector;
struct Transform2d
{
    float x{};
    float y{};

    struct Layer
    {
        int8_t main{};
        int8_t sub{};
    } layer;

    static Transform2d          fromArgs(const Gng2D::ArgsVector&);
    static const ComponentArgs* argsInfo();
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
