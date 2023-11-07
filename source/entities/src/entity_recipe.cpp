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

EntityRecipe EntityRecipe::redXRecipe(entt::registry& reg)
{
    using namespace entt::literals;

    EntityRecipe recipe(reg);

    auto spriteType = entt::resolve("Sprite"_hs);
    GNG2D_ASSERT(spriteType, "Cannot resolve sprite");

    auto sprite = spriteType.construct("red_x"_hs.value());
    GNG2D_ASSERT(sprite, "Cannot construct sprite");

    auto tPosType = entt::resolve("Transform2d"_hs);
    GNG2D_ASSERT(tPosType, "Cannot resolve tPosType");

    auto transformPos = tPosType.construct(0.0f, 0.0f, 0, 0);
    GNG2D_ASSERT(transformPos, "Cannot construct transformPos");

    recipe.components.emplace_back(std::move(sprite));
    recipe.components.emplace_back(std::move(transformPos));

    return recipe;
}
