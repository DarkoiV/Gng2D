#pragma once
#include <entt/entt.hpp>
#include <variant>

namespace Gng2D {
using HashedString = entt::hashed_string;
using StringHash   = entt::hashed_string::hash_type;
consteval StringHash operator""_hash(const char* str, std::size_t)
{
    return entt::hashed_string::value(str);
}

using Err     = std::string_view;
using Success = std::monostate;

template <typename T>
struct ErrOr : std::variant<T, Err>
{
    using std::variant<T, Err>::variant;

    inline bool isErr() const { return std::holds_alternative<Err>(*this); }
    inline bool isSuccess() const { return std::holds_alternative<T>(*this); }

    inline const Err& asErr() const { return std::get<Err>(*this); }
    inline T&         asValue() { return std::get<T>(*this); }
    inline const T&   asValue() const { return std::get<T>(*this); }
};
using ErrOrSuccess = ErrOr<Success>;
} // namespace Gng2D
