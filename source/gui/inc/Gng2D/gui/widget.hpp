#pragma once
#include <entt/entt.hpp>
#include "Gng2D/components/position.hpp"

namespace Gng2D
{
struct Widget
{
    Widget(entt::registry&, Position);
    Widget(entt::registry&, Widget& parent, RelativePosition);
    Widget(const Widget&)   = delete;
    Widget(Widget&&)        = delete;
    virtual ~Widget();

    const Position  getAbsolutePos()    const;
    const Position  getPos() const;

    void setPos(float x, float y);


protected:
    entt::registry& reg;
    entt::entity    root;
};
}

