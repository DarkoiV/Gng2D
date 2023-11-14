#pragma once
#include "Gng2D/commons/args_vector.hpp"
#include "Gng2D/commons/component_args.hpp"
#include <entt/entt.hpp>
#include <vector>

namespace Gng2D {
struct InputListener
{
    using Action = entt::hashed_string::hash_type;
    using Signal = entt::sigh<void(entt::entity, Action)>;
    std::vector<Action> actions;
    Signal              signal;

    static InputListener        fromArgs(const Gng2D::ArgsVector&);
    static const ComponentArgs* argsInfo();
};
} // namespace Gng2D
