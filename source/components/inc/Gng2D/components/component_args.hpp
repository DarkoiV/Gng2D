#pragma once
#include "entt/core/type_info.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace Gng2D {
struct ComponentArg
{
    using AcceptedTypes = std::vector<std::reference_wrapper<const entt::type_info>>;

    std::string   name;
    std::string   description;
    AcceptedTypes acceptedTypes;
    bool          required;
};
using ComponentArgs = std::vector<ComponentArg>;

inline std::ostream& operator<<(std::ostream& o, const ComponentArgs& args)
{
    for (const auto& arg: args)
    {
        o << "\n"
          << arg.name << ": " << '"' << arg.description << '"'
          << (arg.required ? " (required)" : " (optional)");
    }
    return o;
}
} // namespace Gng2D
