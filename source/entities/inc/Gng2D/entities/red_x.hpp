#pragma once
#include "Gng2D/entities/entity_recipe.hpp"

namespace Gng2D {
inline EntityRecipe redX(entt::registry* reg)
{
    EntityRecipe recipe(reg);
    using namespace entt::literals;
    using Arg = ArgsVector::NamedArg;
    recipe.addComponent("Info"_hs, {
                                       Arg{"name"_hs, "Red X"}
    });

    recipe.addComponent("Sprite"_hs, {
                                         Arg{"sprite"_hs, "red_x"_hs.value()}
    });

    recipe.addComponent("Transform2d"_hs, {
                                              Arg{"x"_hs, 10},
                                              Arg{"y"_hs, 23}
    });

    recipe.addComponent("TransformLayer"_hs, {
                                                 Arg{"main"_hs, 0},
                                                 Arg{ "sub"_hs, 3}
    });
    return recipe;
}
} // namespace Gng2D
