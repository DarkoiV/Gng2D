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

    struct Layer
    {
        int8_t main{};
        int8_t sub{};
    } layer;

    static Transform2d              fromArgs(const ArgsVector&);
    static const ComponentMetaInfo* metaInfo();

    using MetaFactory = entt::meta_factory<Transform2d>;
    static MetaFactory registerData(MetaFactory);
};

namespace detail {
struct Position
{
    float x;
    float y;

    struct Layer
    {
        int16_t main;
        int16_t sub;

        std::strong_ordering operator<=>(const Layer&) const = default;
    } layer;
};
} // namespace detail
} // namespace Gng2D
