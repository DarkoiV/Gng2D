#pragma once
#include <entt/entt.hpp>

namespace Gng2D {
struct ComponentUserdata
{
    entt::handle owner;
    void*        ptr;
};
}; // namespace Gng2D
