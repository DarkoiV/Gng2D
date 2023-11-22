#include "Gng2D/scene/entity_renderer.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/core/global.hpp"

using Gng2D::EntityRenderer;
using Gng2D::detail::Position;

#define SORT_SIGNAL &EntityRenderer::markForSorting

EntityRenderer::EntityRenderer(entt::registry& r)
    : reg(r)
{
    // clang-format off
    entt::sigh_helper{reg}
        .with<Position>()
            .on_construct<SORT_SIGNAL>(this)
            .on_update<SORT_SIGNAL>(this)
            .on_destroy<SORT_SIGNAL>(this);
    // clang-format on
    LOG::INFO("EntityRenderer created");
}

EntityRenderer::~EntityRenderer()
{
    reg.on_construct<Position>().disconnect(this);
    reg.on_update<Position>().disconnect(this);
    reg.on_destroy<Position>().disconnect(this);
}

void EntityRenderer::operator()(SDL_Renderer* r)
{
    if (needsSorting) sortRenderables();
    const int midXOffset = GLOBAL::RENDER_WIDTH / 2;
    const int midYOffset = GLOBAL::RENDER_HEIGHT / 2;

    renderables.each(
        [r, this, midYOffset, midXOffset](Sprite& sprite, Position& pos)
    {
        SDL_Rect dstRect;
        dstRect.w = sprite.srcRect.w;
        dstRect.h = sprite.srcRect.h;
        dstRect.x = static_cast<int>(pos.x) - dstRect.w / 2 + midXOffset;
        dstRect.y = static_cast<int>(-pos.y) - dstRect.h / 2 + midYOffset;
        SDL_RenderCopy(r, sprite.texture, &sprite.srcRect, &dstRect);
    });
}

void EntityRenderer::markForSorting()
{
    needsSorting = true;
}

void EntityRenderer::sortRenderables()
{
    renderables.sort(
        [&](entt::entity lhs, entt::entity rhs)
    {
        return renderables.get<detail::Position>(lhs).layer <
               renderables.get<detail::Position>(rhs).layer;
    });
    needsSorting = false;
}
