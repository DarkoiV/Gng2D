#include "Gng2D/scene/assets.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/core/global.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using Gng2D::Assets;

Gng2D::Sprite Assets::getSprite(const entt::hashed_string::hash_type id)
{
    //    auto it = sprites.find(id);
    //    if (it != sprites.end())
    //    {
    //        return it->second;
    //    }
    //
    return getSprite(id);
}
