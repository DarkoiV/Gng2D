#pragma once
#include "Gng2D/commons/luna/stack.hpp"
#include "Gng2D/components/meta/properties_macro.hpp"
#include <compare>
#include <cstdint>
#include <entt/entt.hpp>
#include <optional>

namespace Gng2D {
struct ArgsVector;
struct Transform2d
{
    component_property_name(Transform2d);

    float x{};
    float y{};

    static void onCreate(entt::registry&, entt::entity);
    static void onUpdate(entt::registry&, entt::entity);
    static void onDelete(entt::registry&, entt::entity);

    static int lua__index(Luna::Stack, Luna::TypeVector);
    static int lua__newindex(Luna::Stack, Luna::TypeVector);

    static std::optional<Transform2d> fromArgs(const ArgsVector&, entt::registry::context&);

    using MetaFactory = entt::meta_factory<Transform2d>;
    static void registerData(MetaFactory);
};

struct TransformLayer
{
    component_property_name(TransformLayer);

    int8_t main;
    int8_t sub;

    static void onCreate(entt::registry&, entt::entity);
    static void onUpdate(entt::registry&, entt::entity);
    static void onDelete(entt::registry&, entt::entity);

    static int lua__index(Luna::Stack, Luna::TypeVector);
    static int lua__newindex(Luna::Stack, Luna::TypeVector);

    static std::optional<TransformLayer> fromArgs(const ArgsVector&, entt::registry::context&);

    using MetaFactory = entt::meta_factory<TransformLayer>;
    static void registerData(MetaFactory);
};

namespace detail {
struct Position
{
    component_property_name(Position);
    component_property_is_detail;

    float x;
    float y;

    static void onCreate(entt::registry&, entt::entity);
    static void onUpdate(entt::registry&, entt::entity);
};

struct Layer
{
    component_property_name(Layer);
    component_property_is_detail;

    int16_t main;
    int16_t sub;

    static void onCreate(entt::registry&, entt::entity);
    static void onUpdate(entt::registry&, entt::entity);

    std::strong_ordering operator<=>(const Layer&) const = default;
};
} // namespace detail
} // namespace Gng2D
