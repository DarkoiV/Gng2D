#include "Gng2D/components/transform.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "util_macros.hpp"
#include <entt/entt.hpp>

using Gng2D::Transform2d;
using Arg   = Gng2D::ComponentArg;
using Datum = Gng2D::ComponentDatum;
using entt::type_id;
using namespace entt::literals;

// TRANSFORM 2D ///////////////////////////////////////////////////////////////////////////////////
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
    Datum{.id = "x"_hs, .name = "x", .type = Gng2D::FIELD_TYPE::FLOAT},
    Datum{.id = "y"_hs, .name = "y", .type = Gng2D::FIELD_TYPE::FLOAT},
};

const inline static Gng2D::ComponentMetaInfo
    TRANSFORM2D_META{.id   = entt::hashed_string::value("Transform2d"),
                     .name = "Transform2d",
                     .args = TRANSFORM2D_ARGS,
                     .data = TRANSFORM2D_DATA};

std::optional<Transform2d> Transform2d::fromArgs(const Gng2D::ArgsVector& args,
                                                 const entt::registry::context&)
{
    Gng2D::Transform2d transform{};
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("x", transform.x);
            COMP_ARG_CASE("y", transform.y);
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

// TRANSFORM LAYER ////////////////////////////////////////////////////////////////////////////////
using Gng2D::TransformLayer;
// clang-format off
const inline static Gng2D::ComponentArgs TRANSFORMLAYER_ARGS{
    Arg{.name          = "main",
        .description   = "value of main layer transform relative to parent, 0 by default",
        .acceptedTypes = {type_id<float>()},
        .required      = false},
    Arg{.name          = "sub",
        .description   = "value of sub layer transform relative to parent, 1 by default",
        .acceptedTypes = {type_id<float>()},
        .required      = false},
};
// clang-format on

const inline static Gng2D::ComponentData TRANSFORMLAYER_DATA{
    Datum{.id = "main"_hs, .name = "main", .type = Gng2D::FIELD_TYPE::INTEGER},
    Datum{ .id = "sub"_hs,  .name = "sub", .type = Gng2D::FIELD_TYPE::INTEGER},
};

const inline static Gng2D::ComponentMetaInfo
    TRANSFORMLAYER_META{.id   = entt::hashed_string::value("TransformLayer"),
                        .name = "TransformLayer",
                        .args = TRANSFORMLAYER_ARGS,
                        .data = TRANSFORMLAYER_DATA};

std::optional<TransformLayer> TransformLayer::fromArgs(const Gng2D::ArgsVector& args,
                                                       const entt::registry::context&)
{
    Gng2D::TransformLayer transform{.main = 0, .sub = 1};
    for (auto&& [id, arg]: args)
    {
        switch (id)
        {
            COMP_ARG_CASE("main", transform.main);
            COMP_ARG_CASE("sub", transform.sub);
            UNKNOWN_ARG_CASE;
        }
    }

    return transform;
}

TransformLayer::MetaFactory TransformLayer::registerData(MetaFactory factory)
{
    factory.data<&TransformLayer::main>("main"_hs)
        .prop("max"_hs, INT8_MAX)
        .prop("min"_hs, INT8_MIN);
    factory.data<&TransformLayer::sub>("sub"_hs).prop("max"_hs, INT8_MAX).prop("min"_hs, INT8_MIN);
    return factory;
}

const Gng2D::ComponentMetaInfo* TransformLayer::metaInfo()
{
    return &TRANSFORMLAYER_META;
}
