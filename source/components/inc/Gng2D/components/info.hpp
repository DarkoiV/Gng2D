#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/meta/component_meta_info.hpp"
#include <string>

namespace Gng2D {
struct Info
{
    std::string name;

    static Info                     fromArgs(const ArgsVector&);
    static const ComponentMetaInfo* metaInfo();

    using MetaFactory = entt::meta_factory<Info>;
    static MetaFactory registerData(MetaFactory);
};
} // namespace Gng2D
