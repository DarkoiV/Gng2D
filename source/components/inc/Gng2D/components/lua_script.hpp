#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/luna/type.hpp"
#include "Gng2D/components/meta/component_meta_info.hpp"

namespace Gng2D {
struct LuaScript
{
    std::string    scriptName;
    Luna::TableRef entityEnv;

    static std::optional<LuaScript> fromArgs(const ArgsVector&, entt::registry::context&);
    static const ComponentMetaInfo* metaInfo();

    static void OnCreate(entt::registry&, entt::entity);

    using MetaFactory = entt::meta_factory<LuaScript>;
    static MetaFactory registerData(MetaFactory);
};
} // namespace Gng2D
