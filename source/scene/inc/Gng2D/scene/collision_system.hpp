#pragma once
#include "Gng2D/commons/system_interface.hpp"

namespace Gng2D {
struct CollisionSystem : SystemInterface
{
    CollisionSystem(entt::registry&);
    CollisionSystem(CollisionSystem&)  = delete;
    CollisionSystem(CollisionSystem&&) = delete;
    ~CollisionSystem()                 = default;

    auto operator=(CollisionSystem&)  = delete;
    auto operator=(CollisionSystem&&) = delete;

    void onUpdate() override final;
};
} // namespace Gng2D
