#pragma once
#include "Gng2D/commons/log.hpp"
#include "repository.hpp"

namespace Gng2D {
template <Component::Concept C>
void emplaceComponent(entt::registry* r, entt::entity e, entt::meta_any& c)
{
    const auto& metaInfo = *(C::metaInfo());
    LOG::TRACE("Emplacing:", metaInfo.name);

    r->emplace<C>(e, c.cast<C>());
}

template <Component::Concept Comp>
auto Repository::registerComponent()
{
    using namespace entt::literals;
    auto& metaInfo = *(Comp::metaInfo());
    auto& name     = metaInfo.name;

    auto id = entt::hashed_string::value(name.c_str());
    if (auto it = componentNames.find(id); it != componentNames.end())
    {
        if (it->second == name) LOG::FATAL("Component", name, "already registered");
        else LOG::FATAL("Component hash collision", name, "has the same hash as", it->second);
    }
    else
    {
        LOG::INFO("Registering component:", name, "with id:", id);
        componentNames[id] = name;
    }

    return entt::meta<Comp>()
        .type(id)
        .prop("metaInfo"_hs, Comp::metaInfo())
        .template ctor<&Comp::fromArgs>()
        .template func<&emplaceComponent<Comp>>("emplace"_hs);
}
}; // namespace Gng2D
