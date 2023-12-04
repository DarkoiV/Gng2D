#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/types.hpp"
#include <entt/entt.hpp>
#include <vector>

namespace Gng2D {
struct EntityRecipe
{
    EntityRecipe(entt::registry*);
    EntityRecipe(const EntityRecipe&) = default;
    EntityRecipe(EntityRecipe&&)      = default;
    ~EntityRecipe()                   = default;

    EntityRecipe& operator=(const EntityRecipe&) = default;
    EntityRecipe& operator=(EntityRecipe&&)      = default;

    void addComponent(StringHash, const ArgsVector& = {});
    void addComponent(const std::string&, const ArgsVector& = {});

    entt::entity spawn();

  protected:
    entt::registry* reg;

    using CompArgsPair = std::pair<entt::meta_type, ArgsVector>;
    std::vector<CompArgsPair> components;
};
} // namespace Gng2D
