#pragma once
#include "Gng2D/commons/types.hpp"
#include "Gng2D/components/meta/component_args.hpp"
#include <optional>

namespace Gng2D {

enum class FIELD_TYPE { FLOAT, INTEGER, STRING };

struct ComponentDatum
{
    StringHash  id;
    std::string name;
    FIELD_TYPE  type;
};
using ComponentData = std::vector<ComponentDatum>;

struct ComponentMetaInfo
{
    StringHash                   id;
    std::string                  name;
    bool                         isDetail = false;
    std::optional<ComponentArgs> args     = std::nullopt;
    std::optional<ComponentData> data     = std::nullopt;
};
} // namespace Gng2D
