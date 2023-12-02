#pragma once
#include "Gng2D/components/meta/api_calls.hpp"
#include "Gng2D/components/meta/signals.hpp"
#include "repository.hpp"

namespace Gng2D {

template <Component Comp>
void Repository::registerComponent()
{
    using namespace entt::literals;
    auto meta_factory = entt::meta<Comp>().type(Comp::ID).prop("name"_hs, Comp::NAME);

    if constexpr (requires { Comp::IS_DETAIL; }) meta_factory.prop("isDetail"_hs);

    if constexpr (HasRegisteredData<Comp>) Comp::registerData(meta_factory);

    meta_factory.template func<&getComponentRef<Comp>>("getRef"_hs)
        .template func<&patchComponentSignal<Comp>>("patchSignal"_hs);

    if constexpr (IsArgsConstructible<Comp>)
        meta_factory.template func<&emplaceComponent<Comp>>("emplace"_hs);

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
            if constexpr (HasOnSpawnHook<Comp>)
                r->ctx().get<CompSig>(CompSigHook::ON_SPAWN).connect<&Comp::onSpawn>();
        };
        cachedHooks.emplace_back(attach);
    }

    static_assert(std::is_move_assignable_v<Comp> and std::is_move_constructible_v<Comp>);
}
}; // namespace Gng2D
