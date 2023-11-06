#pragma once
#include <entt/entt.hpp>
#include <tuple>
#include <vector>

namespace Gng2D {
struct EntityRecipe
{
    EntityRecipe(entt::registry&);

    entt::entity spawn();

    static EntityRecipe redXRecipe(entt::registry&);

  private:
    entt::registry&             reg;
    std::vector<entt::meta_any> components;
};
} // namespace Gng2D
