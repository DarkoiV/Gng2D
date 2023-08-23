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
    
    reg.on_construct<Gng2D::RelativeLayer>()
        .connect<&RelationSystem::attachLayerToRelativeLayer>();
    reg.on_update<Gng2D::Layer>()
        .connect<&RelationSystem::updateChildrenLayer>();

    reg.on_construct<RelativePosition>()
        .connect<&RelationSystem::attachPositionToRelativePosition>();
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

    reg.on_construct<Gng2D::RelativeLayer>()
        .connect<&RelationSystem::attachLayerToRelativeLayer>();
    reg.on_update<Gng2D::Layer>()
        .disconnect<&RelationSystem::updateChildrenLayer>();

    reg.on_construct<RelativePosition>()
        .disconnect<&RelationSystem::attachPositionToRelativePosition>();
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

    auto* useParentLayer = reg.try_get<Gng2D::RelativeLayer>(child);
    if (not useParentLayer) return;

    if (auto* parentLayer = reg.try_get<Gng2D::Layer>(parentEntity))
    {
        reg.emplace_or_replace<Layer>(child, 
                                      static_cast<int16_t>(parentLayer->main), 
                                      static_cast<int16_t>(parentLayer->sub + useParentLayer->subOffset));
    }
}

void RelationSystem::removeChildFromParent(entt::registry& reg, entt::entity child)
{
    auto& parentEntity = reg.get<Gng2D::Parent>(child).id;
    auto& children = reg.get<Gng2D::Children>(parentEntity).list;
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

void RelationSystem::attachLayerToRelativeLayer(entt::registry& reg, entt::entity entity)
{
    auto parent         = reg.get<Parent>(entity).id;
    auto parentLayer    = reg.get<Layer>(parent);
    auto relLayer       = reg.get<RelativeLayer>(entity);
    reg.emplace<Layer>(entity, 
            static_cast<int16_t>(parentLayer.main), 
            static_cast<int16_t>(parentLayer.sub + relLayer.subOffset));
}

void RelationSystem::updateChildrenLayer(entt::registry& reg, entt::entity parent)
{
    auto* children = reg.try_get<Children>(parent);
    if (not children) return;

    for (const auto child : children->list)
    {
        auto* useParentLayer = reg.try_get<Gng2D::RelativeLayer>(child);
        if (not useParentLayer) break;

        auto parentLayer = reg.get<Layer>(parent);
        reg.emplace_or_replace<Layer>(child, 
                                      static_cast<int16_t>(parentLayer.main), 
                                      static_cast<int16_t>(parentLayer.sub + useParentLayer->subOffset));
    }
}

void RelationSystem::attachPositionToRelativePosition(entt::registry& reg, entt::entity entity)
{
    auto parent     = reg.get<Parent>(entity).id;
    auto parentPos  = reg.get<Position>(parent);
    auto relPos     = reg.get<RelativePosition>(entity);
    reg.emplace<Position>(entity, parentPos.x + relPos.x, parentPos.y + relPos.y);
}

void RelationSystem::updateChildrenPosition(entt::registry& reg, entt::entity parent)
{
    auto* children = reg.try_get<Children>(parent);
    if (not children) return;

    auto parentPos = reg.get<Position>(parent);

    for (const auto child : children->list)
    {
        if (auto relativePos = reg.try_get<RelativePosition>(child))
        {
            reg.patch<Position>(child, [&](auto& pos)
            {
                pos.x = relativePos->x + parentPos.x;
                pos.y = relativePos->y + parentPos.y;
            });
        }
    }
}

