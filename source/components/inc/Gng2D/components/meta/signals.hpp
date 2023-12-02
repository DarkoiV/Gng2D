#pragma once
#include "Gng2D/commons/types.hpp"
#include <entt/entt.hpp>

namespace Gng2D {
using CompSig = entt::sink<entt::sigh<void(entt::registry&, entt::entity)>>;
enum CompSigHook : StringHash {
    ON_SPAWN,
};
} // namespace Gng2D
