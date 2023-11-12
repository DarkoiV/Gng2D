#pragma once
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/sprite.hpp"
#include <entt/entt.hpp>
#include <string>

namespace Gng2D {
struct Repository
{
    static void   loadSprite(const std::string& name,
                             const std::string& path     = "sprites/",
                             const std::string& fileType = ".png");
    static Sprite getSprite(const StringHash);
    static void   freeResources();

  private:
    inline static std::map<StringHash, Sprite>      sprites;
    inline static std::map<StringHash, std::string> spriteNames;
};
} // namespace Gng2D
