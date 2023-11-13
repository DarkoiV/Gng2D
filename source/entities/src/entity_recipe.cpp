#include "Gng2D/entities/entity_recipe.hpp"
#include "Gng2D/commons/assert.hpp"

using Gng2D::EntityRecipe;

EntityRecipe::EntityRecipe(entt::registry& r)
    : reg(r)
{
}

void EntityRecipe::addComponent(StringHash id, const ArgsVector& args)
{
    auto type = entt::resolve(id);
    GNG2D_ASSERT(type, "Cannot resolve:", id);
    auto component = type.construct(args);
    GNG2D_ASSERT(component, "Cannot construct", id);
    components.push_back(std::move(component));
}

void EntityRecipe::addComponent(const std::string& name, const ArgsVector& args)
{
    addComponent(entt::hashed_string::value(name.c_str()), args);
}

entt::entity EntityRecipe::spawn()
{
    using namespace entt::literals;

    auto e = reg.create();
    for (auto& component: components)
    {
        GNG2D_ASSERT(component.type().func("emplace"_hs),
                     "Cannot resolve emplace func of component: ",
                     component.type().prop("name"_hs).value().cast<std::string>());

        // emplace.invoke({}, &reg, e, entt::forward_as_meta(component));
        component.invoke("emplace"_hs.value(), &reg, e, entt::forward_as_meta(component));
    }

    return e;
}
