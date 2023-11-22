#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/components/meta/component_meta_info.hpp"
#include <entt/entt.hpp>

namespace Gng2D {

template <typename C>
concept Component = requires(const C c, const Gng2D::ArgsVector& av) {
    {
        C::metaInfo()
    } -> std::same_as<const ComponentMetaInfo*>;
};

template <Component Comp>
const ComponentMetaInfo* getMetaInfo()
{
    using namespace entt::literals;
    return entt::resolve<Comp>()
        .prop("metaInfo"_hs)
        .value()
        .template cast<const ComponentMetaInfo*>();
}

inline const ComponentMetaInfo* getMetaInfo(StringHash id)
{
    using namespace entt::literals;
    auto* ptr = entt::resolve(id).prop("metaInfo"_hs).value().try_cast<const ComponentMetaInfo*>();
    GNG2D_ASSERT(ptr, "No meta info for", id);
    return *ptr;
}

inline const ComponentMetaInfo* getMetaInfo(entt::meta_type type)
{
    return getMetaInfo(type.id());
}

} // namespace Gng2D
