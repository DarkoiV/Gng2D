#pragma once
#include <entt/entt.hpp>
#include <vector>

namespace Gng2D {
struct InputListener
{
    using Action = entt::hashed_string::hash_type;
    using Signal = entt::sigh<void(entt::entity, Action)>;
    std::vector<Action> actions;
    Signal              signal;
};
} // namespace Gng2D
