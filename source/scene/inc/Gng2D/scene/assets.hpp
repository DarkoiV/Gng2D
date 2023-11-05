#pragma once
#include <map>
#include <string>
#include <optional>
#include <entt/entt.hpp>
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/commons/log.hpp"

namespace Gng2D
{
struct Assets 
{
    static void loadGlobalSprite(const std::string& name, 
                                 const std::string& path = "sprites/",
                                 const std::string& fileType = ".png");
    Sprite      getSprite(const entt::hashed_string::hash_type);

private:
    struct SpriteMap : std::map<entt::hashed_string::hash_type, Sprite>
    {
        using std::map<entt::hashed_string::hash_type, Sprite>::map;
        ~SpriteMap();
    };
    inline static SpriteMap globalSprites;
};
}

