#pragma once
#include <map>
#include <string>
#include <optional>
#include <entt/entt.hpp>
#include "Gng2D/components/sprite.hpp"

namespace Gng2D
{
struct Assets 
{
    static void             loadGlobalSprite(const std::string& name, 
                                             const std::string& path = "sprites/",
                                             const std::string& fileType = ".png");
    std::optional<Sprite>   getSprite(const entt::hashed_string::value_type);

private:
    inline static std::map<entt::hashed_string::value_type, Sprite> globalSprites;
};
}

