#pragma once
#include <cstdint>
#include <compare>

namespace Gng2D
{
struct Layer 
{
    int16_t main;
    int16_t sub;

    auto operator<=>(const Layer&) const = default;   
};

struct LayerTransform
{
    int8_t main;
    int8_t sub;
};
}

