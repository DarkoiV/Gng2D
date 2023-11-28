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

    static std::optional<Transform2d> fromArgs(const ArgsVector&, const entt::registry::context&);
    static const ComponentMetaInfo*   metaInfo();

    using MetaFactory = entt::meta_factory<Transform2d>;
    static MetaFactory registerData(MetaFactory);
};

struct TransformLayer
{
    int8_t main;
    int8_t sub;

    static std::optional<TransformLayer> fromArgs(const ArgsVector&,
                                                  const entt::registry::context&);
    static const ComponentMetaInfo*      metaInfo();

    using MetaFactory = entt::meta_factory<TransformLayer>;
    static MetaFactory registerData(MetaFactory);
};

namespace detail {
struct Position
{
    float x;
    float y;
};

struct Layer
{
    int16_t main;
    int16_t sub;

    std::strong_ordering operator<=>(const Layer&) const = default;
};
} // namespace detail
} // namespace Gng2D
