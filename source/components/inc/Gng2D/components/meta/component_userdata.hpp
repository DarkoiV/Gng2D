#pragma once
#include "Gng2D/commons/assert.hpp"
#include <entt/entt.hpp>

namespace Gng2D {
struct ComponentUserdata
{
    ComponentUserdata(entt::entity e, entt::meta_any&& component)
        : owner(e)
        , compHandle(std::move(component))
    {
        GNG2D_ASSERT(
            not compHandle.owner(),
            "ComponentUserdata should story only reference to actuall component in registry!");
    }

    entt::meta_any ref() { return compHandle.as_ref(); };

    entt::entity   owner;
    entt::meta_any compHandle;
};
}; // namespace Gng2D
