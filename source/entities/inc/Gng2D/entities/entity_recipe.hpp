#pragma once
#include <vector>
#include <tuple>
#include <entt/entt.hpp>

namespace Gng2D
{
struct EntityRecipe
{
    EntityRecipe(entt::registry&);

    entt::entity spawn();

    static EntityRecipe redXRecipe(entt::registry&);

private:
    entt::registry&             reg;
    std::vector<entt::meta_any> components;
};
}

