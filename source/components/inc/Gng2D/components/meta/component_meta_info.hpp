#pragma once
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/meta/component_args.hpp"
#include <optional>

namespace Gng2D {
struct ComponentMetaInfo
{
    StringHash                   id;
    std::string                  name;
    std::optional<ComponentArgs> args = std::nullopt;
};
} // namespace Gng2D
