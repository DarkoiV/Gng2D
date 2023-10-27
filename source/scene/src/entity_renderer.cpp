#include "Gng2D/scene/entity_renderer.hpp"
#include "Gng2D/core/global.hpp"
#include "Gng2D/core/log.hpp"
#include "Gng2D/components/layer.hpp"

using Gng2D::EntityRenderer;

EntityRenderer::EntityRenderer(entt::registry& r)
    : reg(r)
{
    connectSortOnChange<Sprite>();
    connectSortOnChange<Layer>();
    LOG::INFO("Created entity renderer");
}

EntityRenderer::~EntityRenderer()
{
    disconnectSortOnChange<Sprite>();
    disconnectSortOnChange<Layer>();
}

void EntityRenderer::operator()(SDL_Renderer* r)
{
    if (needsSorting) sortRenderables();
    const int  midXOffset = GLOBAL::RENDER_WIDTH  / 2;
    const int  midYOffset = GLOBAL::RENDER_HEIGHT / 2;

    renderables.each([r, this, midYOffset, midXOffset](Sprite& sprite, Position& pos)
    {
        SDL_Rect    dst;
        dst.w = sprite.srcRect.w;
        dst.h = sprite.srcRect.h;
        dst.x = static_cast<int>(pos.x) - dst.w / 2 + midXOffset;
        dst.y = static_cast<int>(pos.y) - dst.h / 2 + midYOffset;
        SDL_RenderCopy(r, sprite.texture, &sprite.srcRect, &dst);
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

