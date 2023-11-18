#pragma once
#include "entt/core/type_info.hpp"
#include <iomanip>
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
    unsigned nameLen = 0;
    unsigned dscLen  = 0;
    for (const auto& arg: args)
    {
        nameLen = std::max((unsigned)arg.name.size(), nameLen);
        dscLen  = std::max((unsigned)arg.description.size(), dscLen);
    }
    nameLen += 2;
    dscLen  += 2;
    for (const auto& arg: args)
    {
        o << "\n"
          << std::setw(nameLen) << arg.name << ": " << std::setw(dscLen) << std::left
          << std::quoted(arg.description) << (arg.required ? " (required)" : " (optional)")
          << std::right;
    }
    return o;
}
} // namespace Gng2D
