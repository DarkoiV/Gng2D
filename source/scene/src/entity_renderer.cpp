#include "Gng2D/scene/entity_renderer.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/core/log.hpp"
#include "Gng2D/components/layer.hpp"

using Gng2D::EntityRenderer;

#define SORT_SIGNAL &EntityRenderer::needsSorting

EntityRenderer::EntityRenderer(entt::registry& r)
    : reg(r)
{
    entt::sigh_helper{reg}
        .with<Sprite>()
            .on_construct<SORT_SIGNAL>(this)
            .on_destroy<SORT_SIGNAL>(this)
        .with<Position>()
            .on_construct<SORT_SIGNAL>(this)
            .on_destroy<SORT_SIGNAL>(this)
        .with<Layer>()
            .on_construct<SORT_SIGNAL>(this)
            .on_update<SORT_SIGNAL>(this)
            .on_destroy<SORT_SIGNAL>(this);
    LOG::INFO("Created entity renderer");
}

EntityRenderer::~EntityRenderer()
{
    reg.on_construct<Sprite>().disconnect<SORT_SIGNAL>(this);
    reg.on_destroy<Sprite>().disconnect<SORT_SIGNAL>(this);

    reg.on_construct<Position>().disconnect<SORT_SIGNAL>(this);
    reg.on_destroy<Position>().disconnect<SORT_SIGNAL>(this);

    reg.on_construct<Layer>().disconnect<SORT_SIGNAL>(this);
    reg.on_update<Layer>().disconnect<SORT_SIGNAL>(this);
    reg.on_destroy<Layer>().disconnect<SORT_SIGNAL>(this);
}

void EntityRenderer::operator()(SDL_Renderer* r)
{
    if (needsSorting) sortRenderables();
    const int  midXOffset = GLOBAL::RENDER_WIDTH  / 2;
    const int  midYOffset = GLOBAL::RENDER_HEIGHT / 2;

    renderables.each([r, this, midYOffset, midXOffset](Sprite& sprite, Position& pos)
    {
        SDL_Rect dstRect;
        dstRect.w = sprite.srcRect.w;
        dstRect.h = sprite.srcRect.h;
        dstRect.x = static_cast<int>(pos.x) - dstRect.w / 2 + midXOffset;
        dstRect.y = static_cast<int>(pos.y) - dstRect.h / 2 + midYOffset;
        SDL_RenderCopy(r, sprite.texture, &sprite.srcRect, &dstRect);
    });
}

void EntityRenderer::markForSorting()
{
    needsSorting = true;
}

void EntityRenderer::sortRenderables()
{
    renderables.sort([&](entt::entity lhs, entt::entity rhs)
    {
        bool leftHasLayer  = reg.all_of<Layer>(lhs);
        bool rightHasLayer = reg.all_of<Layer>(rhs);
        if (not rightHasLayer) return false;
        if (not leftHasLayer)  return true;
        return reg.get<Layer>(lhs) < reg.get<Layer>(rhs);
    });
    needsSorting = false;
}

