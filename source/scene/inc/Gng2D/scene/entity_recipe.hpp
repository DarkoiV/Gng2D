#pragma once
#include <entt/entt.hpp>

namespace Gng2D {
struct EntityRecipe
{
    EntityRecipe(entt::registry&);
    entt::entity spawn();

  private:
    entt::registry& reg;
};
} // namespace Gng2D
