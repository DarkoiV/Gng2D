#pragma once
#include <cstdint>
#include <compare>

namespace Gng2D
{
struct Layer
{
    int16_t main{0};
    int16_t sub{0};

    auto operator<=>(const Layer&) const = default;
};

struct RelativeLayer
{
    int16_t subOffset{1};
};
}

