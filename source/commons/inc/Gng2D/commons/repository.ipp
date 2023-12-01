#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"
#include "repository.hpp"

namespace Gng2D {
namespace detail {
template <Component Comp>
void emplaceComponent(entt::registry* r, entt::entity e, ArgsVector* av)
{
    using namespace entt::literals;
    auto componentOpt = Comp::fromArgs(*av, r->ctx());
    if (componentOpt) r->emplace<Comp>(e, std::move(*componentOpt));
    else [[unlikely]] LOG::ERROR("Failed to initialzie component,", Comp::NAME);
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
    auto meta_factory = entt::meta<Comp>().type(Comp::ID).prop("name"_hs, Comp::NAME);

    if constexpr (requires { Comp::IS_DETAIL; }) meta_factory.prop("isDetail"_hs);

    if constexpr (HasRegisteredData<Comp>) Comp::registerData(meta_factory);

    meta_factory.template func<&detail::getComponentRef<Comp>>("getRef"_hs)
        .template func<&detail::patchComponentSignal<Comp>>("patchSignal"_hs);

    if constexpr (IsArgsConstructible<Comp>)
        meta_factory.template func<&detail::emplaceComponent<Comp>>("emplace"_hs);

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
