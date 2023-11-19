#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/component_meta_info.hpp"
#include <entt/entt.hpp>

namespace Gng2D::Component {

template <typename C>
concept Concept = requires(const C c, const Gng2D::ArgsVector& av) {
    {
        C::metaInfo()
    } -> std::same_as<const ComponentMetaInfo*>;
};

template <Concept Comp>
const ComponentMetaInfo* getMetaInfo()
{
    using namespace entt::literals;
    return entt::resolve<Comp>()
        .prop("metaInfo"_hs)
        .value()
        .template cast<const ComponentMetaInfo*>();
}

} // namespace Gng2D::Component
