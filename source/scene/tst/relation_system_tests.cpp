#include <gtest/gtest.h>
#include "Gng2D/components/layer.hpp"
#include "Gng2D/components/position.hpp"
#include "Gng2D/components/relationship.hpp"
#include "Gng2D/scene/relation_system.hpp"

struct RelationSystemTests : ::testing::Test 
{
    RelationSystemTests()
    {
        parent      = reg.create();
        child       = reg.create();
        childTwo    = reg.create();
    }


    entt::registry reg;
    Gng2D::RelationSystem sut{reg};

    entt::entity parent;
    entt::entity child;
    entt::entity childTwo;
};


TEST_F(RelationSystemTests, AddingParentComponent_AsignsEntityToChildrenListInParentEntity)
{
    reg.emplace<Gng2D::Parent>(child, parent);
    const auto& children = reg.get<Gng2D::Children>(parent);

    ASSERT_EQ(children.list.size(), 1);
    ASSERT_EQ(children.list.at(0), child);

    reg.emplace<Gng2D::Parent>(childTwo, parent);

    ASSERT_EQ(children.list.size(), 2);
    ASSERT_EQ(children.list.at(1), childTwo);
}

TEST_F(RelationSystemTests, DestroyingChildrenEntity_RemovesItFromChildrenListInParentEntity)
{
    reg.emplace<Gng2D::Parent>(child, parent);
    reg.emplace<Gng2D::Parent>(childTwo, parent);
    const auto& children = reg.get<Gng2D::Children>(parent);
    
    ASSERT_EQ(children.list.size(), 2);
    reg.destroy(child);
    ASSERT_EQ(children.list.size(), 1);
}

TEST_F(RelationSystemTests, DestroyingParentEntity_DestroysAllChildrenEntities)
{
    reg.emplace<Gng2D::Parent>(child, parent);
    reg.emplace<Gng2D::Parent>(childTwo, parent);
    
    reg.destroy(parent);
    ASSERT_FALSE(reg.valid(child));
    ASSERT_FALSE(reg.valid(childTwo));
}

struct RelativeLayerTests : RelationSystemTests
{
    RelativeLayerTests()
    {
        reg.emplace<Gng2D::Layer>(parent, int16_t{100});
        reg.emplace<Gng2D::Parent>(child, parent);
        reg.emplace<Gng2D::Parent>(childTwo, parent);
    }

    const int16_t offsetOne = 10;
    const int16_t offsetTwo = -20;
};

TEST_F(RelativeLayerTests, AttachingRelativeLayerComponent_AttachesLayerComponentOffsetToParentLayer)
{
    reg.emplace<Gng2D::RelativeLayer>(child, offsetOne);
    reg.emplace<Gng2D::RelativeLayer>(childTwo, offsetTwo);
    const auto& layerOne        = reg.get<Gng2D::Layer>(child);
    const auto& layerTwo        = reg.get<Gng2D::Layer>(childTwo);
    const auto& parentLayer     = reg.get<Gng2D::Layer>(parent);

    ASSERT_EQ(layerOne.main, parentLayer.main);
    ASSERT_EQ(layerTwo.main, parentLayer.main);
    ASSERT_EQ(layerOne.sub, parentLayer.sub + offsetOne);
    ASSERT_EQ(layerTwo.sub, parentLayer.sub + offsetTwo);
}

TEST_F(RelativeLayerTests, ChangingParentLayer_ChangesChildrenLayers)
{
    reg.emplace<Gng2D::RelativeLayer>(child, offsetOne);
    reg.emplace<Gng2D::RelativeLayer>(childTwo, offsetTwo);
    const auto& layerOne        = reg.get<Gng2D::Layer>(child);
    const auto& layerTwo        = reg.get<Gng2D::Layer>(childTwo);
    const auto& parentLayer     = reg.get<Gng2D::Layer>(parent);

    reg.replace<Gng2D::Layer>(parent, int16_t{77}, int16_t{35});
    ASSERT_EQ(parentLayer.main, 77);
    ASSERT_EQ(parentLayer.sub, 35);

    ASSERT_EQ(layerOne.main, parentLayer.main);
    ASSERT_EQ(layerTwo.main, parentLayer.main);
    ASSERT_EQ(layerOne.sub, parentLayer.sub + offsetOne);
    ASSERT_EQ(layerTwo.sub, parentLayer.sub + offsetTwo);
}

