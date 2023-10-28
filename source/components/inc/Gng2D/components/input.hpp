#pragma once
#include <vector>
#include <tuple>
#include <compare>
#include <entt/entt.hpp>

namespace Gng2D
{
struct KeyboardInput 
{
    using InputId   = entt::hashed_string::hash_type;
    using Handler   = entt::delegate<void(entt::registry&, entt::entity)>;
    struct InputHandlerPair 
    {
        InputId id;
        Handler handler;
        friend std::weak_ordering operator<=>(const InputHandlerPair& lhs, const InputHandlerPair& rhs)
        {
            return lhs.id <=> rhs.id;
        }
        friend std::weak_ordering operator<=>(const InputId lhs, const InputHandlerPair& rhs)
        {
            return lhs <=> rhs.id;
        }
    };
    std::vector<InputHandlerPair> handledInputs;
};
}

