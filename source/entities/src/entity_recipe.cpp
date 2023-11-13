#include "Gng2D/entities/entity_recipe.hpp"
#include "Gng2D/commons/assert.hpp"

using Gng2D::EntityRecipe;

EntityRecipe::EntityRecipe(entt::registry& r)
    : reg(r)
{
}

entt::entity EntityRecipe::spawn()
{
    using namespace entt::literals;

    auto e = reg.create();
    for (auto& component: components)
    {
        auto emplace = component.type().func("emplace"_hs);
        GNG2D_ASSERT(emplace, "Cannot resolve emplace func");

        // emplace.invoke({}, &reg, e, entt::forward_as_meta(component));
        component.invoke("emplace"_hs.value(), &reg, e, entt::forward_as_meta(component));
    }

    return e;
}
