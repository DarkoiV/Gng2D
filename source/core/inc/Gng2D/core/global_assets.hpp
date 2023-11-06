#pragma once
#include <string>
#include "Gng2D/components/sprite.hpp"

namespace Gng2D
{
struct GlobalAssets 
{
    static void     loadGlobalSprite(const std::string& name, 
                                     const std::string& path = "sprites/",
                                     const std::string& fileType = ".png");
    static Sprite   getGlobalSprite(const entt::hashed_string::hash_type);
    static void     deleteAllSprites();

private:
    inline static std::map<entt::hashed_string::hash_type, Sprite> sprites;
};
}

