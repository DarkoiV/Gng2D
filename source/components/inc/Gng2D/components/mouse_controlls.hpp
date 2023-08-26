#pragma once
#include <entt/entt.hpp>
#include "Gng2D/types/v2d.hpp"
#include "Gng2D/types/modfunc.hpp"

namespace Gng2D
{
struct Hoverable
{
    Hoverable(ModFunc enter, ModFunc leave, V2d dimension = {0, 0})
        : onEnterHover(enter)
        , onLeaveHover(leave) 
        , dimension(dimension)
    {
    }
   
    ModFunc onEnterHover;
    ModFunc onLeaveHover;
    V2d     dimension;
};
struct UseSpriteHoverArea {};

struct Clickable
{
    ModFunc lmbCallback;
    ModFunc rmbCallback;

    void leftClick(entt::registry& reg, entt::entity e)
    {
        if (lmbCallback) lmbCallback(reg, e);
    }

    void rightClick(entt::registry& reg, entt::entity e)
    {
        if (rmbCallback) rmbCallback(reg, e);
    }
};
}

