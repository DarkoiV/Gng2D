#pragma once
#include "Gng2D/commons/luna/type.hpp"
#include "Gng2D/components/meta/properties_macro.hpp"
#include <entt/entt.hpp>
#include <optional>

namespace Gng2D {
struct ArgsVector;
struct LuaScript
{
    component_property_name(LuaScript);

    std::string    scriptName;
    Luna::TableRef entityEnv;

    static void onSpawn(entt::registry&, entt::entity);

    static std::optional<LuaScript> fromArgs(const ArgsVector&, entt::registry::context&);

    using MetaFactory = entt::meta_factory<LuaScript>;
    static void registerData(MetaFactory);
};
} // namespace Gng2D
