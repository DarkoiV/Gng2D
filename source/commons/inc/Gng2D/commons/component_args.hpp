#pragma once
#include "entt/core/type_info.hpp"
#include <string_view>
#include <vector>

namespace Gng2D {
struct ComponentArg
{
    using AcceptedTypes = std::vector<std::reference_wrapper<const entt::type_info>>;

    std::string_view name;
    std::string_view description;
    AcceptedTypes    acceptedTypes;
    bool             required;
};
using ComponentArgs = std::vector<ComponentArg>;
} // namespace Gng2D
