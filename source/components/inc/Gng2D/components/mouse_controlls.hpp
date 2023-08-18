#pragma once
#include <functional>
#include <entt/entt.hpp>
#include "Gng2D/types/v2d.hpp"

namespace Gng2D
{
struct Hoverable
{
    using ModFunc = std::function<void(entt::registry&, entt::entity)>;

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
    using Callback = std::function<void(entt::registry&, entt::entity)>;

    Clickable(Callback lmb)
        : leftButtonCallback(lmb) {}

    Callback leftButtonCallback;
};
}

