#include "Gng2D/scene/relation_system.hpp"
#include "Gng2D/components/relationship.hpp"
#include "gtest/gtest.h"

struct RelationSystemTest : ::testing::Test
{

    entt::registry        reg;
    Gng2D::RelationSystem SUT{reg};

    entt::entity parent = reg.create();
    entt::entity child1 = reg.create();
    entt::entity child2 = reg.create();
    entt::entity child3 = reg.create();

    void emplaceChildren()
    {
        reg.emplace<Gng2D::Parent>(child1, parent);
        reg.emplace<Gng2D::Parent>(child2, parent);
        reg.emplace<Gng2D::Parent>(child3, parent);
    }
};

TEST_F(RelationSystemTest, AddingParentComponentToChild_CreatesChildrenComponentInParent)
{
    ASSERT_FALSE(reg.any_of<Gng2D::Children>(parent));
    emplaceChildren();
    ASSERT_TRUE(reg.any_of<Gng2D::Children>(parent));
}

TEST_F(RelationSystemTest, AddingParentComponentToChild_AddsChildToParentChildrenList)
{
    emplaceChildren();

    auto children = reg.get<Gng2D::Children>(parent);
    ASSERT_TRUE(std::count(children.list.begin(), children.list.end(), child1));
    ASSERT_TRUE(std::count(children.list.begin(), children.list.end(), child2));
    ASSERT_TRUE(std::count(children.list.begin(), children.list.end(), child3));
}

TEST_F(RelationSystemTest, DestroyingChil_WillDeleteItFromParentChildrenList)
{
    emplaceChildren();
    reg.destroy(child1);
    reg.destroy(child3);

    auto children = reg.get<Gng2D::Children>(parent);
    ASSERT_FALSE(std::count(children.list.begin(), children.list.end(), child1));
    ASSERT_FALSE(std::count(children.list.begin(), children.list.end(), child3));

    ASSERT_TRUE(std::count(children.list.begin(), children.list.end(), child2));
}

TEST_F(RelationSystemTest, DestroyingParent_WillDestroyAllItsChildren)
{
    emplaceChildren();
    reg.destroy(parent);

    ASSERT_FALSE(reg.valid(child1));
    ASSERT_FALSE(reg.valid(child2));
    ASSERT_FALSE(reg.valid(child3));
}

