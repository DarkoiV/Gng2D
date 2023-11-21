#pragma once
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/meta/component_args.hpp"

namespace Gng2D {
struct ComponentMetaInfo
{
    StringHash    id;
    std::string   name;
    ComponentArgs args;
};
} // namespace Gng2D
