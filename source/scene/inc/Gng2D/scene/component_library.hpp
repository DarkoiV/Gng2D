#pragma once
#include <entt/entt.hpp>

namespace Gng2D
{
struct ComponentLibrary
{
    ComponentLibrary(entt::registry& reg);

    template<typename Component>
    static void emplace(entt::registry*, entt::entity, entt::meta_any&);

    template<typename Component>
    static auto registerComponent(entt::hashed_string name);

    entt::meta_type getMeta(entt::hashed_string);

private: 
    entt::registry& reg;

    // INIT
    static void init();
    inline static bool initialized{false};
};

template<typename Component>
void ComponentLibrary::emplace(entt::registry* reg, entt::entity e, entt::meta_any& metaComp)
{
    reg->emplace<Component>(e, metaComp.cast<Component>());
}

template<typename Component>
auto ComponentLibrary::registerComponent(entt::hashed_string name)
{
    using namespace entt::literals;
    return entt::meta<Component>()
        .type(name.value())
        .ctor()
        .template func<&ComponentLibrary::emplace<Component>>("emplace"_hs);
}
}

