#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/components/info.hpp"
#include "Gng2D/components/meta/util_funcs.hpp"
#include "Gng2D/components/transform.hpp"
#include "gtest/gtest.h"

namespace Luna = Gng2D::Luna;
using Gng2D::EntityLuaApi;

struct LuaApiTest : ::testing::Test
{
    static void SetUpTestSuite()
    {
        Gng2D::Repository::registerComponent<Gng2D::Transform2d>();
        Gng2D::Repository::registerComponent<Gng2D::Info>();
    }
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

TEST_F(LuaApiTest, addComponent_canAddRegisteredComponentToEntity)
{
    auto e = reg.create();
    ASSERT_FALSE(reg.any_of<Gng2D::Transform2d>(e));
    ASSERT_FALSE(reg.any_of<Gng2D::Info>(e));

    luna.doString(
        "function emplaceTransform(self) \n"
        "  addComponent(self, 'Transform2d', {['x'] = 11}) \n"
        "end");

    luna.doString(
        "function emplaceInfo(self) \n"
        "  addComponent(self, 'Info', {['name'] = 'coolEntity'}) \n"
        "end");
    auto stack = luna.getStack();
    stack.pushGlobal("emplaceTransform");
    stack.callFunctionFS({(Luna::Integer)e});

    stack.pushGlobal("emplaceInfo");
    stack.callFunctionFS({(Luna::Integer)e});

    ASSERT_TRUE(reg.all_of<Gng2D::Transform2d>(e));
    ASSERT_EQ(reg.get<Gng2D::Transform2d>(e).x, 11);
    ASSERT_EQ(reg.get<Gng2D::Transform2d>(e).y, 0);

    ASSERT_TRUE(reg.all_of<Gng2D::Info>(e));
    ASSERT_EQ(reg.get<Gng2D::Info>(e).name, "coolEntity");
}

TEST_F(LuaApiTest, component__indexCanAccessComponentDataInsideLuaScript)
{
    auto e = reg.create();
    reg.emplace<Gng2D::Transform2d>(e, TRANSFORM2D_X, TRANSFORM2D_Y);
    reg.emplace<Gng2D::Info>(e, "eName");
    luna.doString(
        "function readTransform(self) \n"
        "  local transform = getComponent(self, 'Transform2d')  \n"
        "  local x = transform.x \n"
        "  local y = transform.y \n"
        "  return x, y \n"
        "end");
    luna.doString(
        "function readInfo(self) \n"
        "  local info = getComponent(self, 'Info') \n"
        "  return info.name \n"
        "end");

    auto stack = luna.getStack();

    stack.pushGlobal("readTransform");
    stack.callFunctionFS({Luna::Integer(e)});
    ASSERT_TRUE(stack.read(-1).isFloat());
    ASSERT_TRUE(stack.read(-2).isFloat());
    ASSERT_EQ(stack.read(-2).asFloat(), TRANSFORM2D_X);
    ASSERT_EQ(stack.read(-1).asFloat(), TRANSFORM2D_Y);

    stack.pushGlobal("readInfo");
    stack.callFunctionFS({Luna::Integer(e)});
    ASSERT_TRUE(stack.read(-1).isString());
    ASSERT_EQ(stack.read(-1), "eName");
}

TEST_F(LuaApiTest, component__newindexCanSetComponentDataInsideLuaScript)
{
    auto  e         = reg.create();
    auto& transform = reg.emplace<Gng2D::Transform2d>(e);
    auto& info      = reg.emplace<Gng2D::Info>(e, "namev1");
    ASSERT_EQ(transform.x, 0.f);
    ASSERT_EQ(transform.y, 0.f);
    ASSERT_EQ(info.name, "namev1");

    luna.doString(
        "function changeTransform(self, x, y) \n"
        "  local transform = getComponent(self, 'Transform2d') \n"
        "  transform.x = x \n"
        "  transform.y = y \n"
        "end");
    luna.doString(
        "function changeName(self, newName) \n"
        "  local info = getComponent(self, 'Info') \n"
        "  info.name = newName \n"
        "end");

    auto stack = luna.getStack();

    stack.pushGlobal("changeTransform");
    stack.callFunctionFS({Luna::Integer(e), 91u, -22.f});
    ASSERT_EQ(transform.x, 91.f);
    ASSERT_EQ(transform.y, -22.f);

    stack.pushGlobal("changeName");
    stack.callFunctionFS({Luna::Integer(e), "namev2"});
    ASSERT_EQ(info.name, "namev2");
}
