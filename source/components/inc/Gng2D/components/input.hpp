#pragma once
#include <vector>
#include <tuple>
#include <compare>
#include <entt/entt.hpp>

namespace Gng2D
{
struct KeyPress 
{
    using Action    = entt::hashed_string::hash_type;
    using Handler   = entt::delegate<void(entt::registry&, entt::entity, Action)>;
    std::vector<Action> actions;
    Handler             handler;
};
}

