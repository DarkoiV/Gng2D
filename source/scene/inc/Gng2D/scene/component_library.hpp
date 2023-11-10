#pragma once
#include <entt/entt.hpp>

namespace Gng2D {
struct ComponentLibrary
{
    ComponentLibrary(entt::registry& reg);

    template <typename Component>
    auto registerComponent(entt::hashed_string::hash_type id);

    entt::meta_type getMeta(entt::hashed_string);

  private:
    entt::registry& reg;

    template <typename Component>
    static void emplace(entt::registry*, entt::entity, entt::meta_any&);
};

template <typename Component>
void ComponentLibrary::emplace(entt::registry* r, entt::entity e, entt::meta_any& metaComp)
{
    r->emplace<Component>(e, metaComp.cast<Component>());
}

template <typename Component>
auto ComponentLibrary::registerComponent(entt::hashed_string::hash_type id)
{
    using namespace entt::literals;
    return entt::meta<Component>()
        .type(id)
        .template func<&ComponentLibrary::emplace<Component>>("emplace"_hs);
}
} // namespace Gng2D
