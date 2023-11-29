#include "Gng2D/components/relationship.hpp"

using Gng2D::Children;
using Gng2D::Parent;

const static Gng2D::ComponentMetaInfo PARENT_META{.id   = entt::hashed_string::value("Parent"),
                                                  .name = "Parent",
                                                  .args = std::nullopt};

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

const Gng2D::ComponentMetaInfo* Parent::metaInfo()
{
    return &PARENT_META;
};

void Children::onDelete(entt::registry& reg, entt::entity e)
{
    auto& children                = reg.get<Children>(e);
    children.inProccessOfDeletion = true;
    for (auto child: children.list)
        reg.destroy(child);
}

const static Gng2D::ComponentMetaInfo CHILDREN_META{.id   = entt::hashed_string::value("Children"),
                                                    .name = "Children",
                                                    .args = std::nullopt};

const Gng2D::ComponentMetaInfo* Children::metaInfo()
{
    return &CHILDREN_META;
};
