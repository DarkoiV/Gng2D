#pragma once
#include "Gng2D/components/component_args.hpp"
#include <SDL2/SDL.h>

namespace Gng2D {
struct ArgsVector;
struct Sprite
{
    SDL_Texture* texture{};
    SDL_Rect     srcRect{};

    static Sprite               fromArgs(const Gng2D::ArgsVector&);
    static const ComponentArgs* argsInfo();
};
} // namespace Gng2D
