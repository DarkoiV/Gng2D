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
    using Signal    = entt::sigh<void(entt::entity, Action)>;
    std::vector<Action> actions;
    Signal              signal;
};
}
