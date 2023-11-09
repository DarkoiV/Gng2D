#pragma once
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
};

struct Children
{
    std::vector<entt::entity> list;
};
} // namespace Gng2D

