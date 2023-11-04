#pragma once
#include <entt/entt.hpp>

namespace Gng2D
{
struct Position 
{
    float x;
    float y;
};

struct TransformPosition
{
    float x;
    float y;

    static entt::meta_factory<TransformPosition> createMetaInfo();
};
}

