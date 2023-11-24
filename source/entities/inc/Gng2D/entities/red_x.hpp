#pragma once
#include "Gng2D/entities/entity_recipe.hpp"

namespace Gng2D {
struct RedX : EntityRecipe
{
    RedX(entt::registry& r)
        : EntityRecipe(r)
    {
        using namespace entt::literals;
        using Arg = ArgsVector::NamedArg;
        addComponent("Info"_hs, {
                                    Arg{"name"_hs, "Red X"}
        });

        addComponent("Sprite"_hs, {
                                      Arg{"sprite"_hs, "red_x"_hs.value()}
        });

        addComponent("Transform2d"_hs, {
                                           Arg{"x"_hs, 10},
                                           Arg{"y"_hs, 23}
        });

        addComponent("TransformLayer"_hs, {
                                              Arg{"main"_hs, 0},
                                              Arg{ "sub"_hs, 3}
        });
    }
};
} // namespace Gng2D
