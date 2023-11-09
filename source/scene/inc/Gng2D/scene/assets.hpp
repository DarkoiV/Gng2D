#pragma once
#include "Gng2D/core/global_assets.hpp"
#include <entt/entt.hpp>

namespace Gng2D {
struct Assets : GlobalAssets
{
    // TODO impl loadSprite();
    Sprite getSprite(const entt::hashed_string::hash_type);
};
} // namespace Gng2D

