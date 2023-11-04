#include "Gng2D/scene/relation_system.hpp"
#include "Gng2D/components/layer.hpp"
#include "Gng2D/components/relationship.hpp"
#include "Gng2D/components/position.hpp"

using Gng2D::RelationSystem;

RelationSystem::RelationSystem(entt::registry& r)
    : reg(r)
{
    reg.on_construct<Gng2D::Parent>()
        .connect<&RelationSystem::addChildToParent>();
    reg.on_destroy<Gng2D::Parent>()
        .connect<&RelationSystem::removeChildFromParent>();
    reg.on_destroy<Gng2D::Children>()
        .connect<&RelationSystem::destroyAllChildren>();
    
    reg.on_construct<Gng2D::TransformLayer>()
        .connect<&RelationSystem::setLayerFromTransformLayer>();
    reg.on_update<Gng2D::TransformLayer>()
        .connect<&RelationSystem::setLayerFromTransformLayer>();
    reg.on_update<Gng2D::Layer>()
        .connect<&RelationSystem::updateChildrenLayer>();

    reg.on_construct<TransformPosition>()
        .connect<&RelationSystem::setPositionFromTransformPosition>();
    reg.on_update<TransformPosition>()
        .connect<&RelationSystem::setPositionFromTransformPosition>();
    reg.on_update<Gng2D::Position>()
        .connect<&RelationSystem::updateChildrenPosition>();

    reg.ctx().emplace<RelationSystem&>(*this);
}

RelationSystem::~RelationSystem()
{
    reg.on_construct<Gng2D::Parent>()
        .disconnect<&RelationSystem::addChildToParent>();
    reg.on_destroy<Gng2D::Parent>()
        .disconnect<&RelationSystem::removeChildFromParent>();
    reg.on_destroy<Gng2D::Children>()
        .disconnect<&RelationSystem::destroyAllChildren>();

    reg.on_construct<Gng2D::TransformLayer>()
        .disconnect<&RelationSystem::setLayerFromTransformLayer>();
    reg.on_update<Gng2D::TransformLayer>()
        .disconnect<&RelationSystem::setLayerFromTransformLayer>();
    reg.on_update<Gng2D::Layer>()
        .disconnect<&RelationSystem::updateChildrenLayer>();

    reg.on_construct<TransformPosition>()
        .disconnect<&RelationSystem::setPositionFromTransformPosition>();
    reg.on_update<TransformPosition>()
        .disconnect<&RelationSystem::setPositionFromTransformPosition>();
    reg.on_update<Gng2D::Position>()
        .disconnect<&RelationSystem::updateChildrenPosition>();

    reg.ctx().erase<RelationSystem&>();
}

void RelationSystem::addChildToParent(entt::registry& reg, entt::entity child)
{
    auto& parentEntity = reg.get<Gng2D::Parent>(child).id;
    if (not reg.all_of<Children>(parentEntity)) reg.emplace<Children>(parentEntity);

    auto& children = reg.get<Gng2D::Children>(parentEntity).list;
    children.emplace_back(child);

    bool hasTransformLayer = reg.all_of<Gng2D::TransformLayer>(child);
    if (hasTransformLayer) setLayerFromTransformLayer(reg, child);

    bool hasTransformPosition = reg.all_of<Gng2D::TransformPosition>(child);
    if (hasTransformPosition) setPositionFromTransformPosition(reg, child);
}

void RelationSystem::removeChildFromParent(entt::registry& reg, entt::entity child)
{
    auto& parentEntity  = reg.get<Gng2D::Parent>(child).id;
    auto& children      = reg.get<Gng2D::Children>(parentEntity).list;
    std::erase(children, child);
}

void RelationSystem::destroyAllChildren(entt::registry& reg, entt::entity parent)
{
    // Copy so removeChildFromParent won't invalidate iterator
    auto children = reg.get<Gng2D::Children>(parent).list;
    for (const auto child : children)
    {
        reg.destroy(child);
    }
}

void RelationSystem::setLayerFromTransformLayer(entt::registry& reg, entt::entity entity)
{
    auto transformLayer = reg.get<TransformLayer>(entity);
    Gng2D::Layer layer;
    layer.main  = transformLayer.main;
    layer.sub   = transformLayer.sub;

    if (auto* parent = reg.try_get<Parent>(entity))
    {
        auto* parentLayer = reg.try_get<Layer>(parent->id);
        if (parentLayer)
        {
            layer.main  += parentLayer->main;
            layer.sub   += parentLayer->sub;
        }
    }

    reg.emplace_or_replace<Gng2D::Layer>(entity, layer);
}

void RelationSystem::updateChildrenLayer(entt::registry& reg, entt::entity parent)
{
    auto* children = reg.try_get<Children>(parent);
    if (not children) return;

    auto parentLayer = reg.get<Layer>(parent);

    for (const auto child : children->list)
    {
        auto* childTransformLayer = reg.try_get<Gng2D::TransformLayer>(child);
        if (not childTransformLayer) break;
        reg.emplace_or_replace<Layer>(child, 
                                      static_cast<int16_t>(parentLayer.main + childTransformLayer->main), 
                                      static_cast<int16_t>(parentLayer.sub + childTransformLayer->sub));
    }
}

void RelationSystem::setPositionFromTransformPosition(entt::registry& reg, entt::entity entity)
{
    auto transformPosition = reg.get<TransformPosition>(entity);
    Gng2D::Position pos;
    pos.x   = transformPosition.x;
    pos.y   = transformPosition.y;

    if (auto* parent = reg.try_get<Parent>(entity))
    {
        auto* parentPosition = reg.try_get<Position>(parent->id);
        if (parentPosition)
        {
            pos.x   += parentPosition->x;
            pos.y   += parentPosition->y;
        }
    }

    reg.emplace_or_replace<Gng2D::Position>(entity, pos);
}

void RelationSystem::updateChildrenPosition(entt::registry& reg, entt::entity parent)
{
    auto* children = reg.try_get<Children>(parent);
    if (not children) return;

    auto parentPos = reg.get<Position>(parent);

    for (const auto child : children->list)
    {
        auto* childTransformPos = reg.try_get<TransformPosition>(child);
        if (not childTransformPos) return;
        reg.patch<Position>(child, [&](auto& pos)
        {
            pos.x = childTransformPos->x + parentPos.x;
            pos.y = childTransformPos->y + parentPos.y;
        });
    }
}

