#include "Gng2D/scene/relation_system.hpp"
#include "Gng2D/components/relationship.hpp"
#include "Gng2D/components/transform2d.hpp"

using Gng2D::RelationSystem;

RelationSystem::RelationSystem(entt::registry& r)
    : reg(r)
{
    reg.on_construct<Parent>().connect<&RelationSystem::addChildToParent>();
    reg.on_destroy<Parent>().connect<&RelationSystem::removeChildFromParent>();
    reg.on_destroy<Children>().connect<&RelationSystem::destroyAllChildren>();

    reg.on_construct<Transform2d>().connect<&RelationSystem::setPositionFromTransform>();
    reg.on_update<Transform2d>().connect<&RelationSystem::setPositionFromTransform>();
    reg.on_update<detail::Position>().connect<&RelationSystem::updateChildrenPosition>();

    reg.ctx().emplace<RelationSystem&>(*this);
}

RelationSystem::~RelationSystem()
{
    reg.on_construct<Parent>().disconnect<&RelationSystem::addChildToParent>();
    reg.on_destroy<Parent>().disconnect<&RelationSystem::removeChildFromParent>();
    reg.on_destroy<Children>().disconnect<&RelationSystem::destroyAllChildren>();

    reg.on_construct<Transform2d>().disconnect<&RelationSystem::setPositionFromTransform>();
    reg.on_update<Transform2d>().disconnect<&RelationSystem::setPositionFromTransform>();
    reg.on_update<detail::Position>().disconnect<&RelationSystem::updateChildrenPosition>();

    reg.ctx().erase<RelationSystem&>();
}

void RelationSystem::addChildToParent(entt::registry& reg, entt::entity child)
{
    auto& parentEntity = reg.get<Parent>(child).id;
    if (not reg.all_of<Children>(parentEntity)) reg.emplace<Children>(parentEntity);

    auto& children = reg.get<Children>(parentEntity).list;
    children.emplace_back(child);

    bool hasTransform2d = reg.all_of<Transform2d>(child);
    if (hasTransform2d) setPositionFromTransform(reg, child);
}

void RelationSystem::removeChildFromParent(entt::registry& reg, entt::entity child)
{
    auto& parentEntity = reg.get<Parent>(child).id;
    auto& children     = reg.get<Children>(parentEntity).list;
    std::erase(children, child);
}

void RelationSystem::destroyAllChildren(entt::registry& reg, entt::entity parent)
{
    // Copy so removeChildFromParent won't invalidate iterator
    auto children = reg.get<Children>(parent).list;
    for (const auto child: children)
    {
        reg.destroy(child);
    }
}

void RelationSystem::setPositionFromTransform(entt::registry& reg, entt::entity entity)
{
    auto transform = reg.get<Transform2d>(entity);

    detail::Position pos;
    pos.x          = transform.x;
    pos.y          = transform.y;
    pos.layer.main = transform.layer.main;
    pos.layer.sub  = transform.layer.sub;

    if (auto* parent = reg.try_get<Parent>(entity))
    {
        auto* parentPos = reg.try_get<detail::Position>(parent->id);
        if (parentPos)
        {
            pos.x          += parentPos->x;
            pos.y          += parentPos->y;
            pos.layer.main += parentPos->layer.main;
            pos.layer.sub  += parentPos->layer.sub;
        }
    }

    reg.emplace_or_replace<detail::Position>(entity, pos);
}

void RelationSystem::updateChildrenPosition(entt::registry& reg, entt::entity parent)
{
    auto* children = reg.try_get<Children>(parent);
    if (not children) return;

    auto parentPos = reg.get<detail::Position>(parent);

    for (const auto child: children->list)
    {
        auto* childTransformPos = reg.try_get<Transform2d>(child);
        if (not childTransformPos) return;
        reg.patch<detail::Position>(child,
                                    [&](auto& pos)
        {
            pos.x = childTransformPos->x + parentPos.x;
            pos.y = childTransformPos->y + parentPos.y;
        });
    }
}

