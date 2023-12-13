#include "Gng2D/components/collider.hpp"
#include "Gng2D/commons/repository.hpp"
#include "gtest/gtest.h"

struct ColliderTest : ::testing::Test
{
    static void SetUpTestSuite() { Gng2D::Repository::registerComponent<Gng2D::Collider>(); }
    static void TearDownTestSuite() { Gng2D::Repository::freeResources(); };

    ColliderTest() { Gng2D::Repository::attachComponentHooks(&reg); }

    entt::registry reg{};
    entt::entity   e1 = reg.create();
};

using Gng2D::Collider;

TEST_F(ColliderTest, CreatingColliderFromArgs)
{
    using namespace entt::literals;
    Gng2D::ArgsVector args;
    args.addArg("group"_hs, "default");
    args.addArg("type"_hs, "Box");
    args.addArg("width", 12.f);
    args.addArg("height", 5);

    auto optCollider = Collider::fromArgs(args, reg.ctx());
    ASSERT_TRUE(optCollider);
}

TEST_F(ColliderTest, InvalidColliderTypeWontCreateCollider)
{
    using namespace entt::literals;
    Gng2D::ArgsVector args;
    args.addArg("group"_hs, "default");
    args.addArg("type"_hs, "Invalid");
    args.addArg("width", 12.f);
    args.addArg("height", 5);

    auto optCollider = Collider::fromArgs(args, reg.ctx());
    ASSERT_FALSE(optCollider);
}

TEST_F(ColliderTest, EmplacingColliderEmplacesDetailTypeCollider)
{
    using namespace entt::literals;
    Gng2D::ArgsVector args;
    args.addArg("group"_hs, "default");
    args.addArg("type"_hs, "Box");
    args.addArg("width", 12.f);
    args.addArg("height", 5);

    auto optCollider = Collider::fromArgs(args, reg.ctx());
    ASSERT_TRUE(optCollider);

    reg.emplace<Collider>(e1, *optCollider);
    ASSERT_TRUE(reg.all_of<Gng2D::detail::BoxCollider>(e1));

    auto& box = reg.get<Gng2D::detail::BoxCollider>(e1);
    ASSERT_EQ(box.width, 12.f);
    ASSERT_EQ(box.height, 5.f);
}

TEST_F(ColliderTest, RemovingColliderRemovesDetailTypeCollider)
{
    using namespace entt::literals;
    Gng2D::ArgsVector args;
    args.addArg("group"_hs, "default");
    args.addArg("type"_hs, "Box");
    args.addArg("width", 12.f);
    args.addArg("height", 5);

    auto optCollider = Collider::fromArgs(args, reg.ctx());
    ASSERT_TRUE(optCollider);

    reg.emplace<Collider>(e1, *optCollider);
    ASSERT_TRUE(reg.all_of<Gng2D::detail::BoxCollider>(e1));

    reg.erase<Collider>(e1);
    ASSERT_FALSE(reg.all_of<Gng2D::detail::BoxCollider>(e1));
}
