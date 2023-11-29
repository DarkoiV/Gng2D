#pragma once
#include "Gng2D/components/meta/component_meta_info.hpp"
#include <compare>
#include <cstdint>

namespace Gng2D {
struct ArgsVector;
struct Transform2d
{
    float x{};
    float y{};

    static void onCreate(entt::registry&, entt::entity);
    static void onUpdate(entt::registry&, entt::entity);
    static void onDelete(entt::registry&, entt::entity);

    static std::optional<Transform2d> fromArgs(const ArgsVector&, entt::registry::context&);
    static const ComponentMetaInfo*   metaInfo();

    using MetaFactory = entt::meta_factory<Transform2d>;
    static MetaFactory registerData(MetaFactory);
};

struct TransformLayer
{
    int8_t main;
    int8_t sub;

    static void onCreate(entt::registry&, entt::entity);
    static void onUpdate(entt::registry&, entt::entity);
    static void onDelete(entt::registry&, entt::entity);

    static std::optional<TransformLayer> fromArgs(const ArgsVector&, entt::registry::context&);
    static const ComponentMetaInfo*      metaInfo();

    using MetaFactory = entt::meta_factory<TransformLayer>;
    static MetaFactory registerData(MetaFactory);
};

namespace detail {
struct Position
{
    float x;
    float y;

    static void onCreate(entt::registry&, entt::entity);
    static void onUpdate(entt::registry&, entt::entity);

    static const ComponentMetaInfo* metaInfo();
};

struct Layer
{
    int16_t main;
    int16_t sub;

    static void onCreate(entt::registry&, entt::entity);
    static void onUpdate(entt::registry&, entt::entity);

    std::strong_ordering operator<=>(const Layer&) const = default;

    static const ComponentMetaInfo* metaInfo();
};
} // namespace detail
} // namespace Gng2D
