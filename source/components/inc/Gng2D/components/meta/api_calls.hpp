#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/components/meta/properties.hpp"

namespace Gng2D {
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
bool hasComponent(entt::registry* r, entt::entity e)
{
    return r->all_of<Comp>(e);
}

template <Component Comp>
void patchComponentSignal(entt::registry* r, entt::entity e)
{
    r->patch<Comp>(e);
}
} // namespace Gng2D
