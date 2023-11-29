#pragma once
#include "Gng2D/components/meta/component_meta_info.hpp"
#include <SDL2/SDL.h>

namespace Gng2D {
struct ArgsVector;
struct Sprite
{
    SDL_Texture* texture{};
    SDL_Rect     srcRect{};

    static std::optional<Sprite>    fromArgs(const ArgsVector&, entt::registry::context&);
    static const ComponentMetaInfo* metaInfo();
};
} // namespace Gng2D
