#include "Gng2D/scene/entity_recipe.hpp"

using Gng2D::EntityRecipe;

EntityRecipe::EntityRecipe(entt::registry& r)
    : reg(r)
{
}

entt::entity EntityRecipe::spawn()
{
    auto e = reg.create();

    return e;
}
