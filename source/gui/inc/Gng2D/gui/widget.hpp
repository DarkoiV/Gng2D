#pragma once
#include <entt/entt.hpp>

namespace Gng2D
{
struct Widget
{
    Widget(entt::registry& r)
        : reg(r)
        , root(reg.create()) {}

    virtual ~Widget()
    {
        reg.destroy(root);
    }

protected:
    entt::registry& reg;
    entt::entity    root;
};
}

