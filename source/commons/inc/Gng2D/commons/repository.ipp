#pragma once
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"
#include "repository.hpp"

namespace Gng2D {
namespace detail {
template <Component C>
void emplaceComponent(entt::registry* r, entt::entity e, entt::meta_any& c)
{
    const auto& metaInfo = *(C::metaInfo());
    LOG::TRACE("Emplacing:", metaInfo.name);

    r->emplace<C>(e, c.cast<C>());
}
} // namespace detail

template <Component Comp>
auto Repository::registerComponent()
{
    using namespace entt::literals;
    auto& metaInfo = *(Comp::metaInfo());
    auto& name     = metaInfo.name;

    // Ensure no hash collision, or repeated registration
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

    auto meta_factory = entt::meta<Comp>().type(id).prop("metaInfo"_hs, Comp::metaInfo());

    constexpr bool HAS_FROM_ARGS_CTOR = requires(const ArgsVector av) { Comp::fromArgs(av); };
    if constexpr (HAS_FROM_ARGS_CTOR)
    {
        GNG2D_ASSERT(metaInfo.args,
                     "Component has fromArgs ctor,"
                     " yet does not define metaInfo for required arguments");
        meta_factory.template ctor<&Comp::fromArgs>()
            .template func<&detail::emplaceComponent<Comp>>("emplace"_hs);
    }

    constexpr bool HAS_REGISTER_DATA = requires(entt::meta_factory<Comp> mf) {
        {
            Comp::registerData(mf)
        } -> std::same_as<entt::meta_factory<Comp>>;
    };
    if constexpr (HAS_REGISTER_DATA)
    {
        meta_factory = Comp::registerData(meta_factory);
    }

    return meta_factory;
}
}; // namespace Gng2D
