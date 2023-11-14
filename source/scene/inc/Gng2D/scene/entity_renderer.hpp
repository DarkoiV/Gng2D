#pragma once
#include "Gng2D/components/sprite.hpp"
#include "Gng2D/components/transform2d.hpp"
#include <SDL2/SDL.h>
#include <entt/entt.hpp>

namespace Gng2D {
struct EntityRenderer
{
    EntityRenderer(entt::registry&);
    EntityRenderer(EntityRenderer&)  = delete;
    EntityRenderer(EntityRenderer&&) = delete;
    ~EntityRenderer();

    auto operator=(EntityRenderer&)  = delete;
    auto operator=(EntityRenderer&&) = delete;

    void operator()(SDL_Renderer*);

  private:
    entt::registry& reg;

    bool needsSorting{false};
    void markForSorting();
    void sortRenderables();

    using Renderables = decltype(reg.group<Sprite, detail::Position>());
    Renderables renderables{reg.group<Sprite, detail::Position>()};
};
} // namespace Gng2D
