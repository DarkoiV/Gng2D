#pragma once
#include <entt/entt.hpp>

template<typename T>
concept HasMetaInfo = requires(T t)
{
    T::createMetaInfo();
};

namespace Gng2D
{
struct ComponentLibrary
{
    ComponentLibrary(entt::registry& reg);

    template<typename Component>
    static void emplace(entt::registry*, entt::entity, entt::meta_any&);

    template<HasMetaInfo Component>
    static auto registerComponent();

    static entt::meta_type getMeta(entt::hashed_string);

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

template<HasMetaInfo Component>
auto ComponentLibrary::registerComponent()
{
    using namespace entt::literals;
    return Component::createMetaInfo()
        .template func<&ComponentLibrary::emplace<Component>>("emplace"_hs);
}
}

