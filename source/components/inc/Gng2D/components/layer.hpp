#pragma once
#include <cstdint>
#include <compare>

namespace Gng2D
{
struct Layer
{
    uint8_t main{0};
    uint8_t sub{127};

    auto operator<=>(const Layer&) const = default;
};

struct RelativeLayer
{
    int8_t subOffset{1};
};
}

