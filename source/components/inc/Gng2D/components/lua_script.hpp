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
    static constexpr auto in_place_delete = true;
    using ScriptConnections               = std::vector<entt::connection>;

    std::string                      scriptName;
    Luna::TableRef                   entityEnv;
    std::optional<Luna::FunctionRef> onUpdate{};
    ScriptConnections                connections{};
    bool                             hasOnHover = false;

    void invokeAction(entt::registry&, HashedString, bool);

    static void onCreate(entt::registry&, entt::entity);
    static void onSpawn(entt::registry&, entt::entity);
    static void onDelete(entt::registry&, entt::entity);

    static std::optional<LuaScript> fromArgs(const ArgsVector&, entt::registry::context&);

    using MetaFactory = entt::meta_factory<LuaScript>;
    static void registerData(MetaFactory);
};
} // namespace Gng2D
