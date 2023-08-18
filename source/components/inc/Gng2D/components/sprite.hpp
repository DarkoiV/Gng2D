#pragma once
#include <SDL2/SDL.h>
#include <string>
#include "Gng2D/types/v2d.hpp"

namespace Gng2D
{
struct Sprite
{
    SDL_Texture*    texture;
    SDL_Rect        srcRect;
};

struct SpriteStretch : V2d
{
};

struct SpriteOpacity 
{
    uint8_t value;
};
}

