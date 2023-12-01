#pragma once
#include "Gng2D/components/meta/properties_macro.hpp"
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include <optional>

namespace Gng2D {
struct ArgsVector;
struct Sprite
{
    component_property_name(Sprite);

    SDL_Texture* texture{};
    SDL_Rect     srcRect{};

    static std::optional<Sprite> fromArgs(const ArgsVector&, entt::registry::context&);
};
} // namespace Gng2D
