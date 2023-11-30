#pragma once
#include <entt/entt.hpp>

namespace Gng2D {
using StringHash = entt::hashed_string::hash_type;
consteval StringHash operator""_hash(const char* str, std::size_t)
{
    return entt::hashed_string::value(str);
}

} // namespace Gng2D
