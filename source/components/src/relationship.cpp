#include "Gng2D/components/relationship.hpp"

using Gng2D::Children;
using Gng2D::Parent;

const inline static Gng2D::ComponentMetaInfo PARENT_META{.id = entt::hashed_string::value("Parent"),
                                                         .name = "Parent",
                                                         .args = std::nullopt};

const inline static Gng2D::ComponentMetaInfo CHILDREN_META{
    .id = entt::hashed_string::value("Children"), .name = "Children", .args = std::nullopt};

const Gng2D::ComponentMetaInfo* Parent::metaInfo()
{
    return &PARENT_META;
};

const Gng2D::ComponentMetaInfo* Children::metaInfo()
{
    return &CHILDREN_META;
};
