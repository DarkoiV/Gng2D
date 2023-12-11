#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/meta/properties_macro.hpp"
#include <optional>

namespace Gng2D {
struct BoxCollider
{
    component_property_name(BoxCollider);

    int width{};
    int height{};

    static std::optional<BoxCollider> fromArgs(const ArgsVector&, entt::registry::context&);

    using MetaFactory = entt::meta_factory<BoxCollider>;
    static void registerData(MetaFactory);
};

struct Collider
{
    component_property_name(Collider);
    enum class Type { BOX } type;
};
} // namespace Gng2D
