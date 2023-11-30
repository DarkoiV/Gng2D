#pragma once
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"
#include "repository.hpp"

namespace Gng2D {
namespace detail {
template <Component Comp>
void emplaceComponent(entt::registry* r, entt::entity e, ArgsVector* av)
{
    using namespace entt::literals;
    const auto& metaInfo = *(Comp::metaInfo());
    LOG::TRACE("Emplacing", metaInfo.name);

    auto componentOpt = Comp::fromArgs(*av, r->ctx());
    if (componentOpt) r->emplace<Comp>(e, std::move(*componentOpt));
    else [[unlikely]] LOG::ERROR("Failed to initialzie component,", metaInfo.name);
}

template <Component Comp>
entt::meta_any getComponentRef(entt::registry* r, entt::entity e)
{
    return entt::forward_as_meta(r->get<Comp>(e));
}

template <Component Comp>
void patchComponentSignal(entt::registry* r, entt::entity e)
{
    r->patch<Comp>(e);
}

} // namespace detail

template <Component Comp>
void Repository::registerComponent()
{
    using namespace entt::literals;
    auto& metaInfo = *(Comp::metaInfo());
    auto& name     = metaInfo.name;

    auto&& context =
        metaInfo.isDetail ? detailComponentCtx : entt::locator<entt::meta_ctx>::value_or();

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

    auto meta_factory = entt::meta<Comp>(context)
                            .type(id)
                            .prop("metaInfo"_hs, Comp::metaInfo())
                            .template func<&detail::getComponentRef<Comp>>("getRef"_hs)
                            .template func<&detail::patchComponentSignal<Comp>>("patchSignal"_hs);

    if constexpr (IsArgsConstructible<Comp>)
    {
        GNG2D_ASSERT(metaInfo.args,
                     "Component has fromArgs ctor,"
                     " yet does not define metaInfo for arguments");
        meta_factory.template func<&detail::emplaceComponent<Comp>>("emplace"_hs);
    }

    if constexpr (HasRegisteredData<Comp>)
    {
        GNG2D_ASSERT(metaInfo.data,
                     "Component has register data,"
                     " yet does not define metaInfo for it");
        meta_factory = Comp::registerData(meta_factory);
    }

    if constexpr (HasAnyHook<Comp>)
    {
        auto attach = [](entt::registry* r)
        {
            if constexpr (HasOnCreateHook<Comp>)
                r->on_construct<Comp>().template connect<&Comp::onCreate>();
            if constexpr (HasOnUpdateHook<Comp>)
                r->on_update<Comp>().template connect<&Comp::onUpdate>();
            if constexpr (HasOnDeleteHook<Comp>)
                r->on_destroy<Comp>().template connect<&Comp::onDelete>();
        };
        cachedHooks.emplace_back(attach);
    }

    static_assert(std::is_move_assignable_v<Comp> and std::is_move_constructible_v<Comp>);
}
}; // namespace Gng2D
