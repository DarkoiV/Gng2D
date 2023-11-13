#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/types.hpp"
#include <entt/entt.hpp>
#include <vector>

namespace Gng2D {
struct EntityRecipe
{
    EntityRecipe(entt::registry&);

    void addComponent(StringHash, const ArgsVector& = {});
    void addComponent(const std::string&, const ArgsVector& = {});

    entt::entity spawn();

  protected:
    entt::registry&             reg;
    std::vector<entt::meta_any> components;
};
} // namespace Gng2D
