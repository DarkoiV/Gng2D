#pragma once
#include "Gng2D/components/meta/component_meta_info.hpp"
#include <entt/entt.hpp>
#include <vector>

namespace Gng2D {
struct Parent
{
    Parent(entt::entity p)
        : id(p)
    {
    }
    entt::entity id;

    static const ComponentMetaInfo* metaInfo();
};

struct Children
{
    std::vector<entt::entity> list;

    static const ComponentMetaInfo* metaInfo();
};
} // namespace Gng2D
