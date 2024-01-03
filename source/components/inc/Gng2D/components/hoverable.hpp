#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/meta/properties_macro.hpp"
#include <optional>

namespace Gng2D {
struct Hoverable
{
    component_property_name(Hoverable);

    float width{};
    float height{};

    static std::optional<Hoverable> fromArgs(const ArgsVector&, entt::registry::context&);

    using MetaFactory = entt::meta_factory<Hoverable>;
    static void registerData(MetaFactory);
};
} // namespace Gng2D
