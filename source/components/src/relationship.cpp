#include "Gng2D/components/relationship.hpp"

using Gng2D::Children;
using Gng2D::Parent;

void Parent::onCreate(entt::registry& reg, entt::entity e)
{
    auto parent = reg.get<Parent>(e);
    if (not reg.all_of<Children>(parent.id()))
    {
        reg.emplace<Children>(parent.id());
    }

    auto& parentChildren = reg.get<Children>(parent.id());
    parentChildren.list.emplace_back(e);
}

void Parent::onDelete(entt::registry& reg, entt::entity e)
{
    auto& parent         = reg.get<Parent>(e);
    auto& parentChildren = reg.get<Children>(parent.id());
    if (not parentChildren.inProccessOfDeletion) std::erase(parentChildren.list, e);
}

void Children::onDelete(entt::registry& reg, entt::entity e)
{
    auto& children                = reg.get<Children>(e);
    children.inProccessOfDeletion = true;
    for (auto child: children.list)
        reg.destroy(child);
}
