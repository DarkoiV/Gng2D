#include "Gng2D/components/transform.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/components/relationship.hpp"
#include "gtest/gtest.h"
#include <entt/entt.hpp>

struct Transform2dTest : ::testing::Test
{
    static void SetUpTestSuite()
    {
        Gng2D::Repository::registerComponent<Gng2D::Transform2d>();
        Gng2D::Repository::registerComponent<Gng2D::Parent>();
        Gng2D::Repository::registerComponent<Gng2D::Children>();
        Gng2D::Repository::registerComponent<Gng2D::detail::Position>();
    }
    static void TearDownTestSuite() { Gng2D::Repository::freeResources(); };
    Transform2dTest() { Gng2D::Repository::attachComponentHooks(&reg); }

    entt::registry reg{};
    entt::entity   parent    = reg.create();
    entt::entity   children1 = reg.create();
    entt::entity   children2 = reg.create();
};

constexpr float PARENT_X = 13.0f;
constexpr float PARENT_Y = 32.0f;

TEST_F(Transform2dTest, AttachingTransformAttachesPosition)
{
    reg.emplace<Gng2D::Transform2d>(parent, PARENT_X, PARENT_Y);
    auto* pos = reg.try_get<Gng2D::detail::Position>(parent);
    ASSERT_TRUE(pos);
    ASSERT_EQ(pos->x, PARENT_X);
    ASSERT_EQ(pos->y, PARENT_Y);
}

TEST_F(Transform2dTest, ChangingTransformChangesPosition)
{
    auto& transform = reg.emplace<Gng2D::Transform2d>(parent, PARENT_X, PARENT_Y);
    auto* pos       = reg.try_get<Gng2D::detail::Position>(parent);
    ASSERT_TRUE(pos);

    constexpr float CHANGE  = 1.7f;
    transform.x            += CHANGE;
    transform.y            += CHANGE;
    reg.patch<Gng2D::Transform2d>(parent);

    ASSERT_EQ(pos->x, PARENT_X + CHANGE);
    ASSERT_EQ(pos->y, PARENT_Y + CHANGE);
}

TEST_F(Transform2dTest, ParentTransform_AffectsChildrenPosition)
{
    constexpr float CHILD_2_X = -8.9f;
    constexpr float CHILD_2_Y = 11.7f;

    reg.emplace<Gng2D::Transform2d>(parent, PARENT_X, PARENT_Y);
    reg.emplace<Gng2D::Parent>(children1, parent);
    reg.emplace<Gng2D::Parent>(children2, parent);
    reg.emplace<Gng2D::Transform2d>(children1, .0f, .0f);
    reg.emplace<Gng2D::Transform2d>(children2, CHILD_2_X, CHILD_2_Y);

    auto& child1pos = reg.get<Gng2D::detail::Position>(children1);
    auto& child2pos = reg.get<Gng2D::detail::Position>(children2);

    ASSERT_EQ(child1pos.x, PARENT_X);
    ASSERT_EQ(child1pos.y, PARENT_Y);
    ASSERT_EQ(child2pos.x, PARENT_X + CHILD_2_X);
    ASSERT_EQ(child2pos.y, PARENT_Y + CHILD_2_Y);

    constexpr float PARENT_X_2 = 88.f;
    constexpr float PARENT_Y_2 = -11.3f;
    reg.replace<Gng2D::Transform2d>(parent, PARENT_X_2, PARENT_Y_2);

    ASSERT_EQ(child1pos.x, PARENT_X_2);
    ASSERT_EQ(child1pos.y, PARENT_Y_2);
    ASSERT_EQ(child2pos.x, PARENT_X_2 + CHILD_2_X);
    ASSERT_EQ(child2pos.y, PARENT_Y_2 + CHILD_2_Y);
}

struct TransformLayerTest : ::testing::Test
{
    static void SetUpTestSuite()
    {
        Gng2D::Repository::registerComponent<Gng2D::TransformLayer>();
        Gng2D::Repository::registerComponent<Gng2D::Parent>();
        Gng2D::Repository::registerComponent<Gng2D::Children>();
        Gng2D::Repository::registerComponent<Gng2D::detail::Layer>();
    }
    static void TearDownTestSuite() { Gng2D::Repository::freeResources(); };
    TransformLayerTest() { Gng2D::Repository::attachComponentHooks(&reg); }

    entt::registry reg{};
    entt::entity   parent    = reg.create();
    entt::entity   children1 = reg.create();
    entt::entity   children2 = reg.create();
};

constexpr int8_t PARENT_MAIN_LAYER = 2;
constexpr int8_t PARENT_SUB_LAYER  = -1;

TEST_F(TransformLayerTest, AttachingTransformAttachesLayer)
{
    reg.emplace<Gng2D::TransformLayer>(parent, PARENT_MAIN_LAYER, PARENT_SUB_LAYER);
    auto* layer = reg.try_get<Gng2D::detail::Layer>(parent);

    ASSERT_TRUE(layer);
    ASSERT_EQ(layer->main, PARENT_MAIN_LAYER);
    ASSERT_EQ(layer->sub, PARENT_SUB_LAYER);
}

TEST_F(TransformLayerTest, ChangingTransformChangesLayer)
{
    auto& transform =
        reg.emplace<Gng2D::TransformLayer>(parent, PARENT_MAIN_LAYER, PARENT_SUB_LAYER);
    auto* layer = reg.try_get<Gng2D::detail::Layer>(parent);
    ASSERT_TRUE(layer);

    constexpr int8_t CHANGE_MAIN  = -11;
    constexpr int8_t CHANGE_SUB   = 38;
    transform.main               += CHANGE_MAIN;
    transform.sub                += CHANGE_SUB;
    reg.patch<Gng2D::TransformLayer>(parent);

    ASSERT_EQ(layer->main, PARENT_MAIN_LAYER + CHANGE_MAIN);
    ASSERT_EQ(layer->sub, PARENT_SUB_LAYER + CHANGE_SUB);
}

TEST_F(TransformLayerTest, ParentTransform_AffectsChildrenLayer)
{
    constexpr int8_t CHILD_2_MAIN = -11;
    constexpr int8_t CHILD_2_SUB  = 2;

    reg.emplace<Gng2D::TransformLayer>(parent, PARENT_MAIN_LAYER, PARENT_SUB_LAYER);
    reg.emplace<Gng2D::Parent>(children1, parent);
    reg.emplace<Gng2D::Parent>(children2, parent);
    reg.emplace<Gng2D::TransformLayer>(children1, (int8_t)0, (int8_t)0);
    reg.emplace<Gng2D::TransformLayer>(children2, CHILD_2_MAIN, CHILD_2_SUB);

    auto& child1layer = reg.get<Gng2D::detail::Layer>(children1);
    auto& child2layer = reg.get<Gng2D::detail::Layer>(children2);

    ASSERT_EQ(child1layer.main, PARENT_MAIN_LAYER);
    ASSERT_EQ(child1layer.sub, PARENT_SUB_LAYER);
    ASSERT_EQ(child2layer.main, PARENT_MAIN_LAYER + CHILD_2_MAIN);
    ASSERT_EQ(child2layer.sub, PARENT_SUB_LAYER + CHILD_2_SUB);

    constexpr int8_t PARENT_MAIN_2 = -81;
    constexpr int8_t PARENT_SUB_2  = 110;
    reg.replace<Gng2D::TransformLayer>(parent, PARENT_MAIN_2, PARENT_SUB_2);

    ASSERT_EQ(child1layer.main, PARENT_MAIN_2);
    ASSERT_EQ(child1layer.sub, PARENT_SUB_2);
    ASSERT_EQ(child2layer.main, PARENT_MAIN_2 + CHILD_2_MAIN);
    ASSERT_EQ(child2layer.sub, PARENT_SUB_2 + CHILD_2_SUB);
}
