#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/components/transform.hpp"
#include "gtest/gtest.h"

namespace Luna = Gng2D::Luna;
using Gng2D::EntityLuaApi;

struct LuaApiTest : ::testing::Test
{
    static void SetUpTestSuite() { Gng2D::Repository::registerComponent<Gng2D::Transform2d>(); }
    static void TearDownTestSuite() { Gng2D::Repository::freeResources(); }

    Luna::State    luna;
    entt::registry reg;
    EntityLuaApi   sut{reg, luna};
};

TEST_F(LuaApiTest, getComponent_canAccessReferenceToComponentThroughMetaAny)
{
    auto e = reg.create();
    reg.emplace<Gng2D::Transform2d>(e, 90.f, 10.f);

    luna.doString(
        "function pushTransform(self) \n"
        "  return getComponent(self, 'Transform2d')  \n"
        "end");

    auto stack = luna.getStack();
    stack.pushGlobal("pushTransform");
    stack.callFunctionFS({Luna::Integer(e)});
    ASSERT_TRUE(stack.read(-1).isUserdata());

    entt::meta_any& transform = *(entt::meta_any*)stack.read(-1).asUserdata().get();
    ASSERT_EQ(transform.data(), reg.try_get<Gng2D::Transform2d>(e));
}
