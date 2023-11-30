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

constexpr float TRANSFORM2D_X = 90.f;
constexpr float TRANSFORM2D_Y = 10.f;

TEST_F(LuaApiTest, getComponent_canAccessReferenceToComponentThroughMetaAny)
{
    auto e = reg.create();
    reg.emplace<Gng2D::Transform2d>(e, TRANSFORM2D_X, TRANSFORM2D_Y);

    luna.doString(
        "function pushTransform(self) \n"
        "  return getComponent(self, 'Transform2d')  \n"
        "end");

    auto stack = luna.getStack();
    stack.pushGlobal("pushTransform");
    stack.callFunctionFS({Luna::Integer(e)});
    ASSERT_TRUE(stack.read(-1).isUserdata());

    entt::meta_any transform = stack.read(-1).asUserdata().toMetaAny();
    auto&          res       = transform.cast<Gng2D::Transform2d&>();
    ASSERT_EQ(res.x, reg.get<Gng2D::Transform2d>(e).x);
    ASSERT_EQ(res.y, reg.get<Gng2D::Transform2d>(e).y);
    res.x                            = 7.f;
    reg.get<Gng2D::Transform2d>(e).y = 101.f;
    ASSERT_EQ(res.x, reg.get<Gng2D::Transform2d>(e).x);
    ASSERT_EQ(res.y, reg.get<Gng2D::Transform2d>(e).y);
}

TEST_F(LuaApiTest, component__indexCanAccessComponentDataInsideLuaScript)
{
    auto e = reg.create();
    reg.emplace<Gng2D::Transform2d>(e, TRANSFORM2D_X, TRANSFORM2D_Y);
    luna.doString(
        "function readTransform(self) \n"
        "  local transform = getComponent(self, 'Transform2d')  \n"
        "  local x = transform.x \n"
        "  local y = transform.y \n"
        "  return x, y \n"
        "end");

    auto stack = luna.getStack();
    stack.pushGlobal("readTransform");
    stack.callFunctionFS({Luna::Integer(e)});
    ASSERT_TRUE(stack.read(-1).isFloat());
    ASSERT_TRUE(stack.read(-2).isFloat());
    ASSERT_EQ(stack.read(-2).asFloat(), TRANSFORM2D_X);
    ASSERT_EQ(stack.read(-1).asFloat(), TRANSFORM2D_Y);
}
