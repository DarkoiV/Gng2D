#pragma once
#include "Gng2D/commons/types.hpp"
#include <entt/entt.hpp>
#include <optional>

namespace Gng2D {
struct ArgsVector;

template <typename C>
concept Component = requires {
    requires std::is_same_v<decltype(C::NAME), const char* const>;
    requires std::is_same_v<decltype(C::ID), const StringHash>;
    requires std::is_move_constructible_v<C>;
    requires std::is_move_assignable_v<C>;
};

template <Component C>
constexpr bool IsArgsConstructible = requires(const ArgsVector& av, entt::registry::context& ctx) {
    {
        C::fromArgs(av, ctx)
    } -> std::same_as<std::optional<C>>;
};

template <Component C>
constexpr bool HasRegisteredData = requires(entt::meta_factory<C> mf) { C::registerData(mf); };

template <Component C>
constexpr bool HasOnCreateHook =
    requires(entt::registry& reg, entt::entity e) { C::onCreate(reg, e); };

template <Component C>
constexpr bool HasOnUpdateHook =
    requires(entt::registry& reg, entt::entity e) { C::onUpdate(reg, e); };

template <Component C>
constexpr bool HasOnDeleteHook =
    requires(entt::registry& reg, entt::entity e) { C::onDelete(reg, e); };

template <Component C>
constexpr bool HasAnyHook = HasOnCreateHook<C> or HasOnUpdateHook<C> or HasOnDeleteHook<C>;
} // namespace Gng2D
