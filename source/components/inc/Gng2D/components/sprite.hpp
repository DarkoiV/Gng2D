#pragma once
#include <SDL2/SDL.h>

namespace Gng2D {
struct Sprite
{
    SDL_Texture* texture{};
    SDL_Rect     srcRect{};
};
} // namespace Gng2D

