#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/luna/type.hpp"
#include "Gng2D/components/meta/component_meta_info.hpp"

namespace Gng2D {
struct LuaScript
{
    std::string    scriptName;
    Luna::TableRef loadedTable;

    static std::optional<LuaScript> fromArgs(const ArgsVector&);
    static const ComponentMetaInfo* metaInfo();

    using MetaFactory = entt::meta_factory<LuaScript>;
    static MetaFactory registerData(MetaFactory);
};
} // namespace Gng2D
