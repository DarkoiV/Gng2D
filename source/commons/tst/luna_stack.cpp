#include "Gng2D/commons/luna/stack.hpp"
#include <gtest/gtest.h>

struct LunaStackTest : ::testing::Test
{
    lua_State* L = luaL_newstate();
};

using namespace Gng2D::Luna;

TEST_F(LunaStackTest, CanDoBasicStackOperations)
{
    Stack stack(L);
    ASSERT_TRUE(lua_gettop(L) == 0);

    stack.pushNil();
    ASSERT_TRUE(lua_gettop(L) == 1);
    ASSERT_TRUE(stack.is(-1) == TYPE::NIL);
    ASSERT_TRUE(stack.is(1) == TYPE::NIL);

    constexpr int INT_CONST = 33;
    stack.pushInt(INT_CONST);
    ASSERT_TRUE(lua_gettop(L) == 2);
    ASSERT_TRUE(stack.is(-1) == TYPE::INT);
    ASSERT_TRUE(stack.is(2) == TYPE::INT);

    constexpr double FLOAT_CONST = 33.21e-5;
    stack.pushFloat(FLOAT_CONST);
    ASSERT_TRUE(lua_gettop(L) == 3);
    ASSERT_TRUE(stack.is(-1) == TYPE::FLOAT);
    ASSERT_TRUE(stack.is(3) == TYPE::FLOAT);

    constexpr char STR_CONST[] = "HELLO";
    stack.pushString(STR_CONST);
    ASSERT_TRUE(lua_gettop(L) == 4);
    ASSERT_TRUE(stack.is(-1) == TYPE::STRING);
    ASSERT_TRUE(stack.is(4) == TYPE::STRING);

    constexpr bool BOOL_CONST = false;
    stack.pushBool(BOOL_CONST);
    ASSERT_TRUE(lua_gettop(L) == 5);
    ASSERT_TRUE(stack.is(-1) == TYPE::BOOL);
    ASSERT_TRUE(stack.is(5) == TYPE::BOOL);

    auto boolValue = stack.read(-1);
    stack.pop(1);
    ASSERT_EQ(boolValue, BOOL_CONST);
    ASSERT_TRUE(lua_gettop(L) == 4);

    auto stringValue = stack.read(-1);
    stack.pop(1);
    ASSERT_EQ(stringValue, STR_CONST);
    ASSERT_TRUE(lua_gettop(L) == 3);

    auto floatValue = stack.read(-1);
    stack.pop(1);
    ASSERT_EQ(floatValue, FLOAT_CONST);
    ASSERT_TRUE(lua_gettop(L) == 2);

    auto intValue = stack.read(-1);
    stack.pop(1);
    ASSERT_EQ(intValue, INT_CONST);
    ASSERT_TRUE(lua_gettop(L) == 1);

    auto nilValue = stack.read(-1);
    stack.pop(1);
    ASSERT_EQ(nilValue, Nil{});
    ASSERT_TRUE(lua_gettop(L) == 0);
}
