#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/meta/properties_macro.hpp"
#include <optional>

namespace Gng2D {
namespace detail {
struct BoxCollider
{
    component_property_name(BoxCollider);
    component_property_is_detail;

    float width{};
    float height{};
};
} // namespace detail

struct Collider
{
    component_property_name(Collider);

    HashedString group;
    enum class Type : StringHash { BOX = "Box"_hash } type;

    float dimension1{};
    float dimension2{};

    static void onCreate(entt::registry&, entt::entity);
    static void onDelete(entt::registry&, entt::entity);

    static std::optional<Collider> fromArgs(const ArgsVector&, entt::registry::context&);
};
} // namespace Gng2D
