#pragma once
#include <entt/entt.hpp>
#include "Gng2D/components/position.hpp"
#include <Gng2D/components/relationship.hpp>

namespace Gng2D
{
struct PositionSystem
{
    PositionSystem(entt::registry&);
    ~PositionSystem();
    void operator()();

protected:
    void updateRelativePosition();

    entt::registry& reg;
    entt::observer  parentPosObserver;

    static void attachPositionToRelativePosition(entt::registry&, entt::entity);
};
}

