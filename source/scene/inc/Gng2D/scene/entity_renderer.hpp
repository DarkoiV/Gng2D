#pragma once
#include <SDL2/SDL.h>
#include <entt/entt.hpp>
#include "Gng2D/components/position.hpp"
#include "Gng2D/components/sprite.hpp"

namespace Gng2D
{
struct EntityRenderer
{
    EntityRenderer(entt::registry&);
    EntityRenderer(EntityRenderer&)                 = delete;
    EntityRenderer(EntityRenderer&&)                = delete;
    EntityRenderer& operator= (EntityRenderer&)     = delete;
    EntityRenderer& operator= (EntityRenderer&&)    = delete;
    ~EntityRenderer();

    void operator()(SDL_Renderer*);
    void markForSorting();

private:
    void    sortRenderables();

    entt::registry& reg;
    bool            needsSorting{false};

    template <typename Component>
    void connectSortOnChange();

    template <typename Component>
    void disconnectSortOnChange();

    using RenderableGroup = decltype(reg.group<Sprite, Position>());
    RenderableGroup     renderables{reg.group<Sprite, Position>()};
};

// Template definitions
template<typename Component>
void EntityRenderer::connectSortOnChange()
{
    reg.on_construct<Component>()
        .template connect<&EntityRenderer::markForSorting>(this);
    reg.on_update<Component>()
        .template connect<&EntityRenderer::markForSorting>(this);
    reg.on_destroy<Component>()
        .template connect<&EntityRenderer::markForSorting>(this);
}

template<typename Component>
void EntityRenderer::disconnectSortOnChange()
{
    reg.on_construct<Component>()
        .template disconnect<&EntityRenderer::markForSorting>(this);
    reg.on_update<Component>()
        .template disconnect<&EntityRenderer::markForSorting>(this);
    reg.on_destroy<Component>()
        .template disconnect<&EntityRenderer::markForSorting>(this);
}
}

