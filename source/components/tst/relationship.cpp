#include "Gng2D/components/relationship.hpp"
#include "Gng2D/commons/repository.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <entt/entt.hpp>

struct RelationshipTest : ::testing::Test
{
    static void SetUpTestSuite()
    {
        Gng2D::Repository::registerComponent<Gng2D::Parent>();
        Gng2D::Repository::registerComponent<Gng2D::Children>();
    }
    static void TearDownTestSuite() { Gng2D::Repository::freeResources(); };

    RelationshipTest() { Gng2D::Repository::attachComponentHooks(&reg); }

    entt::registry reg{};
    entt::entity   parent    = reg.create();
    entt::entity   children1 = reg.create();
    entt::entity   children2 = reg.create();
    entt::entity   children3 = reg.create();
};

TEST_F(RelationshipTest, AttachingParentComponentToChild_AddsChildToChildrenListInParent)
{
    reg.emplace<Gng2D::Parent>(children1, parent);
    reg.emplace<Gng2D::Parent>(children2, parent);
    reg.emplace<Gng2D::Parent>(children3, parent);

    ASSERT_TRUE(reg.all_of<Gng2D::Children>(parent));
    EXPECT_THAT(reg.get<Gng2D::Children>(parent),
                testing::UnorderedElementsAre(children1, children2, children3));
}

TEST_F(RelationshipTest, DestroyingChildren_RemovesItFromParentsChildrenList)
{
    reg.emplace<Gng2D::Parent>(children1, parent);
    reg.emplace<Gng2D::Parent>(children2, parent);
    reg.emplace<Gng2D::Parent>(children3, parent);

    EXPECT_THAT(reg.get<Gng2D::Children>(parent),
                testing::UnorderedElementsAre(children1, children2, children3));

    reg.destroy(children2);
    EXPECT_THAT(reg.get<Gng2D::Children>(parent),
                testing::UnorderedElementsAre(children1, children3));

    reg.destroy(children1);
    EXPECT_THAT(reg.get<Gng2D::Children>(parent), testing::UnorderedElementsAre(children3));

    reg.destroy(children3);
    ASSERT_TRUE(reg.get<Gng2D::Children>(parent).empty());
}

TEST_F(RelationshipTest, DestroyingParent_DestroysChildren)
{
    reg.emplace<Gng2D::Parent>(children1, parent);
    reg.emplace<Gng2D::Parent>(children2, parent);
    reg.emplace<Gng2D::Parent>(children3, parent);

    ASSERT_TRUE(reg.valid(children1));
    ASSERT_TRUE(reg.valid(children2));
    ASSERT_TRUE(reg.valid(children3));
    ASSERT_TRUE(reg.valid(parent));

    reg.destroy(parent);
    ASSERT_FALSE(reg.valid(children1));
    ASSERT_FALSE(reg.valid(children2));
    ASSERT_FALSE(reg.valid(children3));
    ASSERT_FALSE(reg.valid(parent));
}
