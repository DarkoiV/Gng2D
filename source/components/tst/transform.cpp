#include "Gng2D/components/transform.hpp"
#include "Gng2D/commons/repository.hpp"
#include "gtest/gtest.h"
#include <entt/entt.hpp>

struct Transform2dTest : ::testing::Test
{
    static void SetUpTestSuite() { Gng2D::Repository::registerComponent<Gng2D::Transform2d>(); }
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

struct TransformLayerTest : ::testing::Test
{
    static void SetUpTestSuite() { Gng2D::Repository::registerComponent<Gng2D::TransformLayer>(); }
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
