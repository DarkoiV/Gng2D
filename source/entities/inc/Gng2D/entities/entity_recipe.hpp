#pragma once
#include <entt/entt.hpp>
#include <vector>

namespace Gng2D {
struct EntityRecipe
{
    EntityRecipe(entt::registry&);

    entt::entity spawn();

  protected:
    entt::registry&             reg;
    std::vector<entt::meta_any> components;
};
} // namespace Gng2D
