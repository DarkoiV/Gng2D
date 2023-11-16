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

TEST_F(LunaStackTest, CanPushGlobalVariable)
{
    luaL_dostring(L,
                  "GLOBAL_VAR = 332.33 \n"
                  "OTHER_GLOBAL = 'Hello Lua String'");
    Stack stack(L);
    stack.pushGlobal("GLOBAL_VAR");
    stack.pushGlobal("OTHER_GLOBAL");

    ASSERT_EQ(stack.read(-2), 332.33);
    ASSERT_EQ(stack.read(-1), "Hello Lua String");
}

TEST_F(LunaStackTest, CanDoOperationsOnLuaTables)
{
    Stack stack(L);
    ASSERT_EQ(lua_gettop(L), 0);
    stack.newTable();
    ASSERT_EQ(lua_gettop(L), 1);

    constexpr char STRING_KEY[]     = "KEY";
    constexpr int  STRING_KEY_VALUE = 123;
    stack.setTableField(STRING_KEY, STRING_KEY_VALUE);
    ASSERT_EQ(lua_gettop(L), 1);

    constexpr float FLOAT_KEY         = 33.3;
    constexpr char  FLOAT_KEY_VALUE[] = "hey ho";
    stack.setTableField(FLOAT_KEY, FLOAT_KEY_VALUE);
    ASSERT_EQ(lua_gettop(L), 1);

    stack.pushTableField(FLOAT_KEY);
    ASSERT_EQ(stack.read(-1), FLOAT_KEY_VALUE);

    stack.pushTableField(STRING_KEY, -2);
    ASSERT_EQ(stack.read(-1), STRING_KEY_VALUE);

    stack.pushTableField("UNDEFINED_KEY", -3);
    ASSERT_EQ(stack.read(-1), Nil{});

    luaL_dostring(L, "globalTable = {}");
    constexpr char GLOBAL_TABLE_KEY_VALUE[] = "TABLE CAN BE USED AS KEY!";
    stack.pushGlobal("globalTable");
    stack.push(GLOBAL_TABLE_KEY_VALUE);
    stack.setTableFieldFS(-6);

    stack.pop(3);
    stack.pushGlobal("globalTable");
    stack.pushTableFieldFS(-2);
    ASSERT_EQ(stack.read(-1), GLOBAL_TABLE_KEY_VALUE);
}
