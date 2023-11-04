#include "Gng2D/components/position.hpp"

using Gng2D::TransformPosition;

entt::meta_factory<TransformPosition> TransformPosition::createMetaInfo()
{
    using namespace entt::literals;
    return entt::meta<TransformPosition>()
        .type("TransformPosition"_hs)
        .ctor<float, float>();
}

