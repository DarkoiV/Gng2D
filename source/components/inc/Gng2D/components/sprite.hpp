#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

namespace Gng2D
{
struct Sprite 
{
    SDL_Texture*    texture{};
    SDL_Rect        srcRect{};

    static Sprite   loadFromAssets(entt::registry* reg, 
                                   entt::hashed_string::hash_type id);
    static entt::meta_factory<Sprite> createMetaInfo();
};
}

