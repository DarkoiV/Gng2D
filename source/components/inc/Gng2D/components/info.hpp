#pragma once
#include "Gng2D/commons/luna/stack.hpp"
#include "meta/properties_macro.hpp"
#include <entt/entt.hpp>
#include <optional>
#include <string>

namespace Gng2D {
struct ArgsVector;
struct Info
{
    component_property_name(Info);

    std::string name;

    static int lua__index(Luna::Stack, Luna::TypeVector);
    static int lua__newindex(Luna::Stack, Luna::TypeVector);

    static std::optional<Info> fromArgs(const ArgsVector&, entt::registry::context&);

    using MetaFactory = entt::meta_factory<Info>;
    static void registerData(MetaFactory);
};
} // namespace Gng2D
