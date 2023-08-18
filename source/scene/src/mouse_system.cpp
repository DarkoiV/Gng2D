#include "Gng2D/scene/mouse_system.hpp"
#include "Gng2D/components/mouse_controlls.hpp"
#include "Gng2D/components/position.hpp"
#include "Gng2D/components/sprite.hpp"

using namespace Gng2D;

MouseSystem::MouseSystem(entt::registry& reg)
    : reg(reg)
    , spriteHoverObserver(reg, entt::collector.update<Sprite>().where<UseSpriteHoverArea>())
    , stretchHoverObserver(reg, entt::collector.update<SpriteStretch>().where<UseSpriteHoverArea>())
{
    reg.on_construct<UseSpriteHoverArea>()
        .connect<&useSpriteHoverArea>();

    reg.ctx().emplace<MouseSystem&>(*this);
}

MouseSystem::~MouseSystem()
{
    reg.on_construct<UseSpriteHoverArea>()
        .disconnect<&useSpriteHoverArea>();

    reg.ctx().erase<MouseSystem&>();
}

void MouseSystem::motion(SDL_MouseMotionEvent event)
{
    for (auto&& [entity, hoverable, pos] : reg.view<Hoverable, Position>().each())
    {
        const auto beginX   = pos.x - (hoverable.dimension.x/2);
        const auto endX     = pos.x + (hoverable.dimension.x/2);
        const auto beginY   = pos.y - (hoverable.dimension.y/2);
        const auto endY     = pos.y + (hoverable.dimension.y/2);
        bool inside 
            = event.x > beginX and event.x < endX
          and event.y > beginY and event.y < endY;
        if (inside) 
        {
            if (currentlyHovered != entity) enterHover(entity);
            return;
        }
    }
    leaveHover(currentlyHovered);
}

void MouseSystem::button(SDL_MouseButtonEvent event)
{
    auto* clickable = reg.try_get<Clickable>(currentlyHovered);
    if (not clickable) return;
    switch (event.button)
    {
        case SDL_BUTTON_LEFT:
            if (event.state == SDL_PRESSED)
            {
                clickArmedOnEntity = currentlyHovered;
            }
            else if (clickArmedOnEntity == currentlyHovered)
            {
                clickable->leftButtonCallback(reg, currentlyHovered);
            }
            else 
            {
                clickArmedOnEntity = entt::null;
            }
            break;
    }
}

void MouseSystem::operator()()
{
    updateSpriteHoverArea();
}

void MouseSystem::updateSpriteHoverArea()
{
    spriteHoverObserver.each([&](auto entity)
    {
        useSpriteHoverArea(reg, entity);
    });

    stretchHoverObserver.each([&](auto entity)
    {
        useSpriteHoverArea(reg, entity);
    });
}

void MouseSystem::useSpriteHoverArea(entt::registry& reg, entt::entity entity)
{
    auto& hoverable = reg.get<Hoverable>(entity);
    auto& rect = reg.get<Sprite>(entity).srcRect;
    hoverable.dimension = V2d{(float)rect.w, (float)rect.h};
    if (auto* stretch = reg.try_get<SpriteStretch>(entity))
    {
        hoverable.dimension.x *= stretch->x;
        hoverable.dimension.y *= stretch->y;
    }
}

void MouseSystem::enterHover(entt::entity entity)
{
    leaveHover(currentlyHovered);
    reg.get<Hoverable>(entity)
        .onEnterHover(reg, entity);
    currentlyHovered = entity;
}

void MouseSystem::leaveHover(entt::entity entity)
{   
    if (currentlyHovered == entt::null) return;
    reg.get<Hoverable>(entity)
        .onLeaveHover(reg, entity);
    currentlyHovered = entt::null;
}

