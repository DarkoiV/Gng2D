#include "Gng2D/components/transform2d.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "util_macros.hpp"
#include <entt/entt.hpp>

using Gng2D::Transform2d;
using Arg = Gng2D::ComponentArg;
using entt::type_id;

// clang-format off
const inline static Gng2D::ComponentArgs TRANSFORM2D_ARGS{
    Arg{.name          = "x",
        .description   = "x value of transform vector, 0 by default",
        .acceptedTypes = {type_id<float>()},
        .required      = false},
    Arg{.name          = "y",
        .description   = "y value of transform vector, 0 by default",
        .acceptedTypes = {type_id<float>()},
        .required      = false},
    Arg{.name          = "layerMain",
        .description   = "Position on main layer, 0 by default",
        .acceptedTypes = {type_id<int8_t>()},
        .required      = false},
    Arg{.name          = "layerSub",
        .description   = "Position on sub layer, 0 by default",
        .acceptedTypes = {type_id<int8_t>()},
        .required      = false}
};
// clang-format on

const inline static Gng2D::ComponentMetaInfo
    TRANSFORM2D_META{.id   = entt::hashed_string::value("Transform2d"),
                     .name = "Transform2d",
                     .args = TRANSFORM2D_ARGS};

Transform2d Transform2d::fromArgs(const Gng2D::ArgsVector& args)
{
    using namespace entt::literals;
    Gng2D::Transform2d transform{};
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("x", float, transform.x);
            COMP_ARG_CASE("y", float, transform.y);
            COMP_ARG_CASE("layerMain", int8_t, transform.layer.main);
            COMP_ARG_CASE("layerSub", int8_t, transform.layer.sub);
            UNKNOWN_ARG_CASE;
        }
    }

    return transform;
}

const Gng2D::ComponentMetaInfo* Transform2d::metaInfo()
{
    return &TRANSFORM2D_META;
}
