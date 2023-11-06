#pragma once
#include <entt/entt.hpp>

namespace Gng2D {
struct RelationSystem
{
    RelationSystem(entt::registry&);
    ~RelationSystem();

  private:
    entt::registry& reg;

    static void addChildToParent(entt::registry&, entt::entity);
    static void removeChildFromParent(entt::registry&, entt::entity);
    static void destroyAllChildren(entt::registry&, entt::entity);

    static void setLayerFromTransformLayer(entt::registry&, entt::entity);
    static void updateChildrenLayer(entt::registry&, entt::entity);

    static void setPositionFromTransformPosition(entt::registry&, entt::entity);
    static void updateChildrenPosition(entt::registry&, entt::entity);
};
} // namespace Gng2D
