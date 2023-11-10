#pragma once
#include "Gng2D/components/sprite.hpp"
#include <entt/entt.hpp>
#include <string>

namespace Gng2D {
struct GlobalAssets
{
    static void   loadGlobalSprite(const std::string& name,
                                   const std::string& path     = "sprites/",
                                   const std::string& fileType = ".png");
    static Sprite getSprite(const entt::hashed_string::hash_type);
    static void   deleteAllSprites();

  private:
    inline static std::map<entt::hashed_string::hash_type, Sprite> sprites;
};
} // namespace Gng2D
