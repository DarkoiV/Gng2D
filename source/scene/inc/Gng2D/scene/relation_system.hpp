#pragma once
#include <entt/entt.hpp>

namespace Gng2D
{
struct RelationSystem
{
    RelationSystem(entt::registry&);
    ~RelationSystem();
    void operator()();

private:
    entt::registry& reg;
    entt::observer  parentPosObserver;

    void updateRelativePositions();

    static void addChildToParent(entt::registry&, entt::entity);
    static void removeChildFromParent(entt::registry&, entt::entity);
    static void destroyAllChildren(entt::registry&, entt::entity);

    static void attachLayerToRelativeLayer(entt::registry&, entt::entity);
    static void updateChildrenLayer(entt::registry&, entt::entity);

    static void attachPositionToRelativePosition(entt::registry&, entt::entity);
};
}

