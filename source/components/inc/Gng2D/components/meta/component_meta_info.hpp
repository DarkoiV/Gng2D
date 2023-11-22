#pragma once
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/meta/component_args.hpp"
#include <optional>

namespace Gng2D {

enum class INPUT_TYPE { FLOAT, INTEGER, STRING };

struct ComponentDatum
{
    StringHash  id;
    std::string name;
    INPUT_TYPE  type;
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
