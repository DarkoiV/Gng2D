#include "Gng2D/scene/entity_renderer.hpp"
#include "Gng2D/components/rotation.hpp"
#include "Gng2D/components/layer.hpp"

using namespace Gng2D;

EntityRenderer::EntityRenderer(entt::registry& r)
    : reg(r)
{
    connectSortOnChange<Sprite>();
    connectSortOnChange<Layer>();
}

EntityRenderer::~EntityRenderer()
{
    disconnectSortOnChange<Sprite>();
    disconnectSortOnChange<Layer>();
}

void EntityRenderer::operator()(SDL_Renderer* renderer)
{
    if (needsSorting) sortRenderables();
    reg.group<Sprite, Position>().each([renderer, this](entt::entity entity, Sprite& sprite, Position& pos)
    {
        SDL_Rect dstRect;
        uint8_t opacity = 255;
        if (auto* spriteOpacity = reg.try_get<SpriteOpacity>(entity))
        {
            opacity = spriteOpacity->value;
        }
        SDL_SetTextureAlphaMod(sprite.texture, opacity);

        float spriteStretchX = 1.0f;
        float spriteStretchY = 1.0f;
        if (auto* stretch = reg.try_get<SpriteStretch>(entity))
        {
            spriteStretchX = stretch->x;
            spriteStretchY = stretch->y;
        }

        dstRect.w = sprite.srcRect.w * spriteStretchX;
        dstRect.h = sprite.srcRect.h * spriteStretchY;
        dstRect.x = static_cast<int>(pos.x) - dstRect.w / 2;
        dstRect.y = static_cast<int>(pos.y) - dstRect.h / 2;

        double angle = 0;
        if (auto* rotation = reg.try_get<Rotation>(entity))
        {
            angle = rotation->angle;
        }
            
        SDL_RenderCopyEx(renderer, sprite.texture, &sprite.srcRect, 
                         &dstRect, angle, nullptr, SDL_FLIP_NONE);
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
    reg.sort<Rotation, Sprite>();
    reg.sort<SpriteStretch, Sprite>();
    reg.sort<SpriteOpacity, Sprite>();

    needsSorting = false;
}

