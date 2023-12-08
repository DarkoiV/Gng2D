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

TEST_F(LunaStackTest, CanCallFunction)
{
    luaL_dostring(L, "foo = function() varSetFromFoo = 100 end");
    Stack stack(L);
    stack.pushGlobal("foo");
    auto foo = stack.read(-1);
    ASSERT_TRUE(foo.isFunction());

    stack.pushGlobal("varSetFromFoo");
    ASSERT_EQ(stack.read(-1), Nil{});

    constexpr int VOID_RETURN = 0;
    ASSERT_EQ(stack.callFunction(foo.asFunction()), VOID_RETURN);

    stack.pushGlobal("varSetFromFoo");
    ASSERT_EQ(stack.read(-1), 100);
}

TEST_F(LunaStackTest, CanCallFunctionWithMultipleArgsAndReturns)
{
    luaL_dostring(L,
                  "foo = function(x, y, z) \n"
                  "  local sum = x + y + z \n"
                  "  local mul = x * y * z \n"
                  "  return sum, mul \n"
                  "end");
    Stack stack(L);
    stack.pushGlobal("foo");
    auto             foo           = stack.read(-1).asFunction();
    constexpr int    ARGS_NO       = 3;
    constexpr int    RET_NO        = 2;
    constexpr double ARGS[ARGS_NO] = {1.3, 3.2, 9.0};
    ASSERT_EQ(stack.callFunction(foo, {ARGS[0], ARGS[1], ARGS[2]}), RET_NO);
    ASSERT_EQ(stack.read(-1), ARGS[0] * ARGS[1] * ARGS[2]);
    ASSERT_EQ(stack.read(-2), ARGS[0] + ARGS[1] + ARGS[2]);
}

TEST_F(LunaStackTest, CanCallFunctionWithArgsAlreadyOnStack)
{
    luaL_dostring(L,
                  "function foo(x, y, z, d) \n"
                  "  local sum = x + y \n"
                  "  local sum2 = z + d \n"
                  "  return sum, sum2 \n"
                  "end");
    Stack stack(L);
    stack.pushGlobal("foo");
    stack.pushInt(1);
    stack.pushInt(10);
    stack.pushFloat(88.12);
    stack.pushFloat(100.);
    stack.callFunctionFS(4);
    ASSERT_TRUE(stack.read(-1).isFloat());
    ASSERT_TRUE(stack.read(-2).isInteger());
    ASSERT_EQ(stack.read(-1), 88.12 + 100.);
    ASSERT_EQ(stack.read(-2), 1 + 10);
}

TEST_F(LunaStackTest, CanAssignMetaTables)
{
    luaL_dostring(L,
                  "function addToUserdata(userdata) \n"
                  "  return userdata + 5 \n"
                  "end");
    Stack stack(L);
    auto  intRef = stack.newUserdata<int>(123);
    ASSERT_EQ(*(int*)intRef.get(), 123);

    stack.newTable();
    auto table = stack.read(-1).asTable();
    stack.pop(1);

    auto* add = +[](lua_State* L) -> int
    {
        int* value  = (int*)lua_touserdata(L, 1);
        *value     += lua_tointeger(L, 2);
        lua_pushvalue(L, 1);

        return 1;
    };
    lua_pushcclosure(L, add, 0);
    auto __add = stack.read(-1).asFunction();
    stack.pop(1);

    table.set("__add", __add);
    intRef.setMetaTable(table);

    ASSERT_TRUE(stack.read(-1).isUserdata());
    stack.pushGlobal("addToUserdata");
    stack.push(intRef);
    ASSERT_TRUE(lua_pcall(L, 1, 1, 0) == LUA_OK) << lua_tostring(L, -1);
    ASSERT_EQ(*(int*)intRef.get(), 123 + 5);
    ASSERT_TRUE(stack.read(-1).isUserdata());
}
