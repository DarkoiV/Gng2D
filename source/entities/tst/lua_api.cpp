#include "Gng2D/entities/lua_api.hpp"
#include "Gng2D/commons/repository.hpp"
#include "Gng2D/components/info.hpp"
#include "Gng2D/components/lua_script.hpp"
#include "Gng2D/components/meta/component_userdata.hpp"
#include "Gng2D/components/transform.hpp"
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace Luna = Gng2D::Luna;
using Gng2D::EntityLuaApi;

constexpr float TRANSFORM2D_X = 90.f;
constexpr float TRANSFORM2D_Y = 10.f;

struct LuaApiTest : ::testing::Test
{
    static void SetUpTestSuite()
    {
        Gng2D::Repository::registerComponent<Gng2D::Transform2d>();
        Gng2D::Repository::registerComponent<Gng2D::Info>();
        Gng2D::Repository::registerComponent<Gng2D::LuaScript>();
    }
    static void TearDownTestSuite() { Gng2D::Repository::freeResources(); }

    LuaApiTest()
    {
        reg.ctx().emplace_as<Gng2D::CompSig>(Gng2D::CompSigHook::ON_SPAWN, onSpawnSignal);
        Gng2D::Repository::attachComponentHooks(&reg);
        reg.emplace<Gng2D::LuaScript>(e, "custom", entityEnv);
    }

    Luna::State           luna;
    entt::registry        reg;
    EntityLuaApi          sut{reg, luna};
    entt::entity          e         = reg.create();
    Gng2D::Luna::TableRef entityEnv = luna.createTableRef();

    entt::sigh<void(entt::registry&, entt::entity)> onSpawnSignal;
};

TEST_F(LuaApiTest, EntityLuaApi_DuringLuaScriptCosntruction_attachesImplicitSelf)
{
    using namespace Gng2D;
    ASSERT_TRUE(entityEnv.get("Self").isTable());
    ASSERT_EQ(entityEnv.get("Self"), entityEnv);
    ASSERT_EQ(entityEnv.get("entity"_hash), (Luna::Integer)e);
}

TEST_F(LuaApiTest, getComponent_canAccessReferenceToComponentThroughMetaAny)
{
    reg.emplace<Gng2D::Transform2d>(e, TRANSFORM2D_X, TRANSFORM2D_Y);
    luna.doString(
        "function pushTransform() \n"
        "  return Self:getComponent('Transform2d')  \n"
        "end",
        entityEnv);

    auto stack = luna.getStack();
    stack.push(entityEnv.get("pushTransform"));
    stack.callFunctionFS({});
    ASSERT_TRUE(stack.read(-1).isUserdata());

    entt::meta_any transform = stack.read(-1).asUserdata().get<Gng2D::ComponentUserdata>().ref();
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
    ASSERT_FALSE(reg.any_of<Gng2D::Transform2d>(e));
    ASSERT_FALSE(reg.any_of<Gng2D::Info>(e));

    luna.doString(
        "function emplaceTransform() \n"
        "  Self:addComponent('Transform2d', {['x'] = 11}) \n"
        "end",
        entityEnv);

    luna.doString(
        "function emplaceInfo() \n"
        "  Self:addComponent('Info', {['name'] = 'coolEntity'}) \n"
        "end",
        entityEnv);

    auto stack = luna.getStack();
    stack.push(entityEnv.get("emplaceTransform"));
    stack.callFunctionFS({});

    stack.push(entityEnv.get("emplaceInfo"));
    stack.callFunctionFS({});

    ASSERT_TRUE(reg.all_of<Gng2D::Transform2d>(e));
    ASSERT_EQ(reg.get<Gng2D::Transform2d>(e).x, 11);
    ASSERT_EQ(reg.get<Gng2D::Transform2d>(e).y, 0);

    ASSERT_TRUE(reg.all_of<Gng2D::Info>(e));
    ASSERT_EQ(reg.get<Gng2D::Info>(e).name, "coolEntity");
}

TEST_F(LuaApiTest, hasComponentCanCheckWhetherEntityHasGivenComponentAssigned)
{
    luna.doString(
        "function hasTransform() \n"
        "  return Self:hasComponent('Transform2d') \n"
        "end",
        entityEnv);
    auto stack        = luna.getStack();
    auto hasTransform = entityEnv.get("hasTransform");

    stack.callFunction(hasTransform.asFunction(), {Luna::Integer(e)});
    ASSERT_TRUE(stack.read(-1).isBool());
    ASSERT_FALSE(stack.read(-1).asBool());
    stack.pop(1);

    reg.emplace<Gng2D::Transform2d>(e);
    stack.callFunction(hasTransform.asFunction(), {Luna::Integer(e)});
    ASSERT_TRUE(stack.read(-1).isBool());
    ASSERT_TRUE(stack.read(-1).asBool());
}

TEST_F(LuaApiTest, component__indexCanAccessComponentDataInsideLuaScript)
{
    reg.emplace<Gng2D::Transform2d>(e, TRANSFORM2D_X, TRANSFORM2D_Y);
    reg.emplace<Gng2D::Info>(e, "eName");

    luna.doString(
        "function readTransform() \n"
        "  local transform = Self:getComponent('Transform2d')  \n"
        "  local x = transform.x \n"
        "  local y = transform.y \n"
        "  return x, y \n"
        "end",
        entityEnv);
    luna.doString(
        "function readInfo() \n"
        "  local info = Self:getComponent('Info') \n"
        "  return info.name \n"
        "end",
        entityEnv);

    auto stack = luna.getStack();

    stack.push(entityEnv.get("readTransform"));
    stack.callFunctionFS({Luna::Integer(e)});
    ASSERT_TRUE(stack.read(-1).isFloat());
    ASSERT_TRUE(stack.read(-2).isFloat());
    ASSERT_EQ(stack.read(-2).asFloat(), TRANSFORM2D_X);
    ASSERT_EQ(stack.read(-1).asFloat(), TRANSFORM2D_Y);

    stack.push(entityEnv.get("readInfo"));
    stack.callFunctionFS({Luna::Integer(e)});
    ASSERT_TRUE(stack.read(-1).isString());
    ASSERT_EQ(stack.read(-1), "eName");
}

TEST_F(LuaApiTest, component__newindexCanSetComponentDataInsideLuaScript)
{
    auto& transform = reg.emplace<Gng2D::Transform2d>(e);
    auto& info      = reg.emplace<Gng2D::Info>(e, "namev1");

    ASSERT_EQ(transform.x, 0.f);
    ASSERT_EQ(transform.y, 0.f);
    ASSERT_EQ(info.name, "namev1");

    luna.doString(
        "function changeTransform(x, y) \n"
        "  local transform = Self:getComponent('Transform2d') \n"
        "  transform.x = x \n"
        "  transform.y = y \n"
        "end",
        entityEnv);
    luna.doString(
        "function changeName(newName) \n"
        "  local info = Self:getComponent('Info') \n"
        "  info.name = newName \n"
        "end",
        entityEnv);

    auto stack = luna.getStack();

    stack.push(entityEnv.get("changeTransform"));
    stack.callFunctionFS({91u, -22.f});
    ASSERT_EQ(transform.x, 91.f);
    ASSERT_EQ(transform.y, -22.f);

    stack.push(entityEnv.get("changeName"));
    stack.callFunctionFS({"namev2"});
    ASSERT_EQ(info.name, "namev2");
}

TEST_F(LuaApiTest, component__newindexSendsPatchSignal)
{
    reg.emplace<Gng2D::Transform2d>(e, 100.f, 77.f);
    auto& pos = reg.get<Gng2D::detail::Position>(e);
    ASSERT_EQ(pos.x, 100.f);
    ASSERT_EQ(pos.y, 77.f);

    luna.doString(
        "function changeTransform(x, y) \n"
        "  local transform = Self:getComponent('Transform2d') \n"
        "  transform.x = x \n"
        "  transform.y = y \n"
        "end",
        entityEnv);

    auto stack = luna.getStack();
    stack.push(entityEnv.get("changeTransform"));
    stack.callFunctionFS({91u, -22.f});
    ASSERT_EQ(pos.x, 91.f);
    ASSERT_EQ(pos.y, -22.f);
}
