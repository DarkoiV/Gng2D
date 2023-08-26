#pragma once
#include <entt/entt.hpp>
#include <SDL2/SDL.h>

namespace Gng2D
{
struct MouseSystem 
{
    MouseSystem(entt::registry&);
    ~MouseSystem();

    void motion(SDL_MouseMotionEvent);
    void button(SDL_MouseButtonEvent);

    void operator()();

private:
    entt::registry& reg;
    entt::observer  spriteHoverObserver;
    entt::observer  stretchHoverObserver;

    void markForSorting();
    bool needsSorting{false};
    void sortHoverables();

    void updateSpriteHoverArea();
    static void useSpriteHoverArea(entt::registry&, entt::entity);

    entt::entity currentlyHovered{entt::null};
    void enterHover(entt::entity);
    void leaveHover(entt::entity);

    entt::entity leftSelected{entt::null};
    entt::entity rightSelected{entt::null};
};
}

