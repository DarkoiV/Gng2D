#include "Gng2D/entities/recipe.hpp"
#include "Gng2D/commons/assert.hpp"

using Gng2D::EntityRecipe;

EntityRecipe::EntityRecipe(entt::registry* r)
    : reg(r)
{
}

void EntityRecipe::addComponent(StringHash id, const ArgsVector& args)
{
    auto type = entt::resolve(id);
    GNG2D_ASSERT(type, "Cannot resolve:", id);
    components.emplace_back(type, args);
}

void EntityRecipe::addComponent(const std::string& name, const ArgsVector& args)
{
    addComponent(entt::hashed_string::value(name.c_str()), args);
}

entt::entity EntityRecipe::spawn()
{
    using namespace entt::literals;

    auto e = reg->create();
    for (auto&& [component, args]: components)
    {
        auto emplace = component.func("emplace"_hs);
        GNG2D_ASSERT(emplace, "No emplace function for", component.info().name());
        emplace.invoke({}, reg, e, &args);
    }

    return e;
}
