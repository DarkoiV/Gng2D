#include "Gng2D/components/transform.hpp"
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/components/meta/properties.hpp"
#include "Gng2D/components/relationship.hpp"
#include "util_macros.hpp"
#include <entt/entt.hpp>

using namespace entt::literals;

// TRANSFORM 2D ///////////////////////////////////////////////////////////////////////////////////
using Gng2D::Transform2d;
void Transform2d::onCreate(entt::registry& reg, entt::entity e)
{
    onUpdate(reg, e);
}

void Transform2d::onUpdate(entt::registry& reg, entt::entity e)
{
    auto transform = reg.get<Transform2d>(e);

    detail::Position pos;
    pos.x = transform.x;
    pos.y = transform.y;

    if (auto* parent = reg.try_get<Parent>(e))
    {
        auto* parentPos = reg.try_get<detail::Position>(parent->id());
        if (parentPos)
        {
            pos.x += parentPos->x;
            pos.y += parentPos->y;
        }
    }

    reg.emplace_or_replace<detail::Position>(e, pos);
}

void Transform2d::onDelete(entt::registry& reg, entt::entity e)
{
    reg.remove<detail::Position>(e);
}

std::optional<Transform2d> Transform2d::fromArgs(const Gng2D::ArgsVector& args,
                                                 entt::registry::context&)
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

void Transform2d::registerData(MetaFactory factory)
{
    factory.data<&Transform2d::x>("x"_hs)
        .prop(ComponentFieldProperties::FIELD_TYPE, ComponentFieldType::FLOAT)
        .prop(ComponentFieldProperties::FIELD_NAME, "x");

    factory.data<&Transform2d::y>("y"_hs)
        .prop(ComponentFieldProperties::FIELD_TYPE, ComponentFieldType::FLOAT)
        .prop(ComponentFieldProperties::FIELD_NAME, "y");
}

// TRANSFORM LAYER ////////////////////////////////////////////////////////////////////////////////
using Gng2D::TransformLayer;
void TransformLayer::onCreate(entt::registry& reg, entt::entity e)
{
    onUpdate(reg, e);
}

void TransformLayer::onUpdate(entt::registry& reg, entt::entity e)
{
    auto transform = reg.get<TransformLayer>(e);

    detail::Layer layer;
    layer.main = transform.main;
    layer.sub  = transform.sub;

    if (auto* parent = reg.try_get<Parent>(e))
    {
        auto* parentLayer = reg.try_get<detail::Layer>(parent->id());
        if (parentLayer)
        {
            layer.main += parentLayer->main;
            layer.sub  += parentLayer->sub;
        }
    }

    reg.emplace_or_replace<detail::Layer>(e, layer);
}

void TransformLayer::onDelete(entt::registry& reg, entt::entity e)
{
    reg.remove<detail::Layer>(e);
}

std::optional<TransformLayer> TransformLayer::fromArgs(const Gng2D::ArgsVector& args,
                                                       entt::registry::context&)
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

void TransformLayer::registerData(MetaFactory factory)
{
    factory.data<&TransformLayer::main>("main"_hs)
        .prop(ComponentFieldProperties::MAX, INT8_MAX)
        .prop(ComponentFieldProperties::MIN, INT8_MIN)
        .prop(ComponentFieldProperties::FIELD_TYPE, Gng2D::ComponentFieldType::INTEGER)
        .prop(ComponentFieldProperties::FIELD_NAME, "main");
    factory.data<&TransformLayer::sub>("sub"_hs)
        .prop(ComponentFieldProperties::MAX, INT8_MAX)
        .prop(ComponentFieldProperties::MIN, INT8_MIN)
        .prop(ComponentFieldProperties::FIELD_TYPE, Gng2D::ComponentFieldType::INTEGER)
        .prop(ComponentFieldProperties::FIELD_NAME, "sub");
}

// DETAIL POSITIION ///////////////////////////////////////////////////////////////////////////////
using Gng2D::detail::Position;
void Position::onCreate(entt::registry& reg, entt::entity e)
{
    onUpdate(reg, e);
}

void Position::onUpdate(entt::registry& reg, entt::entity e)
{
    auto* children = reg.try_get<Children>(e);
    if (not children) return;

    auto parentPos = reg.get<detail::Position>(e);

    for (const auto child: *children)
    {
        auto* childTransformPos = reg.try_get<Transform2d>(child);
        if (not childTransformPos) continue;

        reg.patch<detail::Position>(child,
                                    [&](auto& pos)
        {
            pos.x = childTransformPos->x + parentPos.x;
            pos.y = childTransformPos->y + parentPos.y;
        });
    }
}

// DETAIL LAYER ///////////////////////////////////////////////////////////////////////////////////
using Gng2D::detail::Layer;
void Layer::onCreate(entt::registry& reg, entt::entity e)
{
    onUpdate(reg, e);
}

void Layer::onUpdate(entt::registry& reg, entt::entity e)
{
    auto* children = reg.try_get<Children>(e);
    if (not children) return;

    auto parentLayer = reg.get<detail::Layer>(e);

    for (const auto child: *children)
    {
        auto* childTransformLayer = reg.try_get<TransformLayer>(child);
        if (not childTransformLayer) continue;

        reg.patch<detail::Layer>(child,
                                 [&](auto& layer)
        {
            layer.main = childTransformLayer->main + parentLayer.main;
            layer.sub  = childTransformLayer->sub + parentLayer.sub;
        });
    }
}
