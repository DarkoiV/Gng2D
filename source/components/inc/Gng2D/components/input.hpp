#pragma once
#include <vector>
#include <tuple>
#include <compare>
#include <entt/entt.hpp>

namespace Gng2D
{
struct KeyPress 
{
    using Id        = entt::hashed_string::hash_type;
    using Callback  = entt::delegate<void(entt::registry&, entt::entity)>;
    struct Handler 
    {
        Id id;
        Callback handler;
        friend std::weak_ordering operator<=>(const Handler& lhs, const Handler& rhs)
        {
            return lhs.id <=> rhs.id;
        }
        friend std::weak_ordering operator<=>(const Id lhs, const Handler& rhs)
        {
            return lhs <=> rhs.id;
        }
    };
    std::vector<Handler> handlers;
};
}

