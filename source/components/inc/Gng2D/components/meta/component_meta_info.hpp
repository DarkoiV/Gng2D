#pragma once
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/meta/component_args.hpp"
#include <optional>

namespace Gng2D {
struct ComponentDatum
{
    StringHash      id;
    std::string     name;
    entt::type_info type;
};
using ComponentData = std::vector<ComponentDatum>;

struct ComponentMetaInfo
{
    StringHash                   id;
    std::string                  name;
    std::optional<ComponentArgs> args = std::nullopt;
    std::optional<ComponentData> data;
};
} // namespace Gng2D
