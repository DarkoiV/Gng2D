#include "Gng2D/components/transform2d.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "util_macros.hpp"
#include <entt/entt.hpp>

using Gng2D::Transform2d;
using Arg   = Gng2D::ComponentArg;
using Datum = Gng2D::ComponentDatum;
using entt::type_id;
using namespace entt::literals;

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
};
// clang-format on

const inline static Gng2D::ComponentData TRANSFORM2D_DATA{
    Datum{.id = "x"_hs, .name = "x", .type = Gng2D::INPUT_TYPE::FLOAT},
    Datum{.id = "y"_hs, .name = "y", .type = Gng2D::INPUT_TYPE::FLOAT},
};

const inline static Gng2D::ComponentMetaInfo
    TRANSFORM2D_META{.id   = entt::hashed_string::value("Transform2d"),
                     .name = "Transform2d",
                     .args = TRANSFORM2D_ARGS,
                     .data = TRANSFORM2D_DATA};

Transform2d Transform2d::fromArgs(const Gng2D::ArgsVector& args)
{
    Gng2D::Transform2d transform{};
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("x", float, transform.x);
            COMP_ARG_CASE("y", float, transform.y);
            UNKNOWN_ARG_CASE;
        }
    }

    return transform;
}

Transform2d::MetaFactory Transform2d::registerData(MetaFactory factory)
{
    return factory.data<&Transform2d::x>("x"_hs).data<&Transform2d::y>("y"_hs);
}

const Gng2D::ComponentMetaInfo* Transform2d::metaInfo()
{
    return &TRANSFORM2D_META;
}
