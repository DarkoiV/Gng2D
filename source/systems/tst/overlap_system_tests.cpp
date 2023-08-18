#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <entt/entt.hpp>
#include "Gng2D/components/position.hpp"
#include "Gng2D/components/colider.hpp"
#include "Gng2D/systems/overlap_system.hpp"
#include "Gng2D/scene/scene.hpp"
#include "Gng2D/types/entity_compositor.hpp"

namespace
{

constexpr auto RADIUS_1_UNIT = 1;
constexpr auto RADIUS_3_UNIT = 3;
}

using ::testing::_;

struct CircleXCircleOverlapSystemTests : public ::testing::Test 
{
    entt::registry registry;
    struct ColliderA : public Gng2D::CircleCollider {};
    struct ColliderB : public Gng2D::CircleCollider {};

    entt::entity CreateEntityA(entt::registry& reg, Gng2D::Position p, Gng2D::CircleCollider cc)
    {
        return Gng2D::EntityCompositor(reg)
            .with<Gng2D::Position>(p) 
            .with<ColliderA>(cc)
            .get();
    };

    entt::entity CreateEntityB(entt::registry& reg, Gng2D::Position p, Gng2D::CircleCollider cc)
    {
        return Gng2D::EntityCompositor(reg)
            .with<Gng2D::Position>(p) 
            .with<ColliderB>(cc)
            .get();
    }
    
    struct MockOverlapSystem : Gng2D::OverlapSystem<ColliderA, ColliderB>
    {
        MockOverlapSystem(entt::registry& reg)
            : Gng2D::OverlapSystem<ColliderA, ColliderB>(reg) {}
        MOCK_METHOD(void, onOverlap, (entt::entity, entt::entity), (override));
    } overlapSystem{registry};
};

TEST_F(CircleXCircleOverlapSystemTests, NoColliders)
{
    EXPECT_CALL(overlapSystem, onOverlap(_,_)).Times(0);
    overlapSystem();
}

TEST_F(CircleXCircleOverlapSystemTests, FourColliders_NoCollision)
{
    CreateEntityA(registry, Gng2D::Position{0,4},  Gng2D::CircleCollider{RADIUS_1_UNIT});
    CreateEntityB(registry, Gng2D::Position{1,6},  Gng2D::CircleCollider{RADIUS_1_UNIT});
    CreateEntityA(registry, Gng2D::Position{10,9}, Gng2D::CircleCollider{RADIUS_3_UNIT});
    CreateEntityB(registry, Gng2D::Position{16,9}, Gng2D::CircleCollider{RADIUS_3_UNIT});

    EXPECT_CALL(overlapSystem, onOverlap(_,_)).Times(0);
    overlapSystem();
}

TEST_F(CircleXCircleOverlapSystemTests, FourColliders_OverlapingCollidersOfSameType_NoCollision)
{
    CreateEntityA(registry, Gng2D::Position{0,4},  Gng2D::CircleCollider{RADIUS_1_UNIT});
    CreateEntityB(registry, Gng2D::Position{5,10}, Gng2D::CircleCollider{RADIUS_1_UNIT});
    CreateEntityA(registry, Gng2D::Position{0,4},  Gng2D::CircleCollider{RADIUS_3_UNIT});
    CreateEntityB(registry, Gng2D::Position{5,10}, Gng2D::CircleCollider{RADIUS_3_UNIT});

    EXPECT_CALL(overlapSystem, onOverlap(_,_)).Times(0);
    overlapSystem();
}

TEST_F(CircleXCircleOverlapSystemTests, FourColliders_OneCollision)
{
    const auto A1Id = 
        CreateEntityA(registry, Gng2D::Position{0,4},  Gng2D::CircleCollider{RADIUS_1_UNIT});
    const auto B1Id =
        CreateEntityB(registry, Gng2D::Position{1,5},  Gng2D::CircleCollider{RADIUS_1_UNIT});
    CreateEntityA(registry, Gng2D::Position{15,4}, Gng2D::CircleCollider{RADIUS_3_UNIT});
    CreateEntityB(registry, Gng2D::Position{1,60}, Gng2D::CircleCollider{RADIUS_3_UNIT});

    EXPECT_CALL(overlapSystem, onOverlap(A1Id, B1Id)).Times(1);
    overlapSystem();
}

TEST_F(CircleXCircleOverlapSystemTests, FourColliders_ThreeCollisions)
{
    const auto A1Id 
        = CreateEntityA(registry, Gng2D::Position{0,5},  Gng2D::CircleCollider{RADIUS_1_UNIT});
    const auto B1Id 
        = CreateEntityB(registry, Gng2D::Position{1,5},  Gng2D::CircleCollider{RADIUS_1_UNIT});
    const auto A2Id 
        = CreateEntityA(registry, Gng2D::Position{3,5},  Gng2D::CircleCollider{RADIUS_3_UNIT});
    const auto B2Id 
        = CreateEntityB(registry, Gng2D::Position{4,10}, Gng2D::CircleCollider{RADIUS_3_UNIT});

    EXPECT_CALL(overlapSystem, onOverlap(A1Id, B1Id)).Times(1);
    EXPECT_CALL(overlapSystem, onOverlap(A2Id, B1Id)).Times(1);
    EXPECT_CALL(overlapSystem, onOverlap(A2Id, B2Id)).Times(1);
    overlapSystem();
}

