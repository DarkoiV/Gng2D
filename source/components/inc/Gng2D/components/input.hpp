#pragma once
#include "Gng2D/components/meta/properties_macro.hpp"
#include <entt/entt.hpp>
#include <vector>

namespace Gng2D {
struct ArgsVector;
struct InputListener
{
    component_property_name(InputListener);

    using Action = entt::hashed_string::hash_type;
    using Signal = entt::sigh<void(entt::entity, Action)>;
    std::vector<Action> actions;
    Signal              signal;

    static InputListener fromArgs(const Gng2D::ArgsVector&, entt::registry::context&);
};
} // namespace Gng2D
