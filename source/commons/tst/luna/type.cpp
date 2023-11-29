#include "Gng2D/commons/luna/type.hpp"
#include "Gng2D/commons/luna/stack.hpp"
#include <gtest/gtest.h>

using namespace Gng2D::Luna;

struct LunaTypeTest : ::testing::Test
{
    lua_State* L = luaL_newstate();
};

TEST_F(LunaTypeTest, TableRefWillKeepTableAlive)
{
    Stack stack(L);
    stack.newTable();
    auto       tref          = stack.read(-1);
    const auto firstTablePtr = lua_topointer(L, -1);
    stack.pop(-1);
    ASSERT_EQ(stack.top(), 0);

    stack.push(tref);
    const auto secondTablePtr = lua_topointer(L, -1);
    ASSERT_EQ(firstTablePtr, secondTablePtr);
}

TEST_F(LunaTypeTest, TableRefCanDoOperationsOnUnderlingTable)
{
    Stack stack(L);
    stack.newTable();
    auto testedTable = stack.read(-1).asTable();

    constexpr char KEY_1[]   = "HELLO KEY";
    constexpr int  KEY_2     = 403;
    constexpr int  VALUE_1   = 13;
    constexpr char VALUE_2[] = "TESTO";
    stack.setTableField(KEY_1, VALUE_1, -1);
    stack.setTableField(KEY_2, VALUE_2, -1);
    stack.newTable();

    auto            tableKey        = stack.read(-1);
    constexpr float TABLE_KEY_VALUE = 3.33;
    stack.pushFloat(TABLE_KEY_VALUE);
    stack.setTableFieldFS();

    constexpr char  KEY_3[] = "KEY Num 3";
    constexpr int   VALUE_3 = 500;
    constexpr float KEY_4   = 42.24;
    constexpr float VALUE_4 = 42.24;
    testedTable.set(KEY_3, VALUE_3);
    testedTable.set(KEY_4, VALUE_4);
    lua_settop(L, 0);

    ASSERT_EQ(testedTable.get(KEY_1), VALUE_1);
    ASSERT_EQ(testedTable.get(KEY_2), VALUE_2);
    ASSERT_EQ(testedTable.get(tableKey), TABLE_KEY_VALUE);
    ASSERT_EQ(testedTable.get(KEY_3), VALUE_3);
    ASSERT_EQ(testedTable.get(KEY_4), VALUE_4);
}

TEST_F(LunaTypeTest, ReferencesToTheSameTableAreEqual)
{
    Stack stack(L);
    stack.newTable();
    auto table = stack.read(-1).asTable();
    lua_setglobal(L, "GLOBAL_TABLE");
    ASSERT_EQ(lua_gettop(L), 0);

    stack.pushGlobal("GLOBAL_TABLE");
    auto globalTable = stack.read(-1).asTable();
    ASSERT_EQ(table, globalTable);
}

TEST_F(LunaTypeTest, CanIterateOverLunaTable)
{
    luaL_dostring(L,
                  "foo = function() return 1 end \n"
                  "x = {} \n"
                  "x[123] = 'table-value' \n"
                  "x.test = 32.91 \n"
                  "x[foo] = x \n");

    Stack stack(L);
    stack.pushGlobal("x");
    ASSERT_TRUE(stack.read(-1).isTable());
    auto tableRef = stack.read(-1).asTable();
    stack.pushGlobal("foo");
    ASSERT_TRUE(stack.read(-1).isFunction());
    auto fooRef = stack.read(-1).asFunction();

    using kvpair        = std::pair<Type, Type>;
    const kvpair PAIR_1 = {123, "table-value"};
    const kvpair PAIR_2 = {"test", 32.91};
    const kvpair PAIR_3 = {fooRef, tableRef};
    int          pair_1_count{0};
    int          pair_2_count{0};
    int          pair_3_count{0};

    for (auto&& pair: tableRef)
    {
        if (pair == PAIR_1) pair_1_count++;
        else if (pair == PAIR_2) pair_2_count++;
        else if (pair == PAIR_3) pair_3_count++;
        else ASSERT_TRUE(false);
    }

    ASSERT_EQ(pair_1_count, 1);
    ASSERT_EQ(pair_2_count, 1);
    ASSERT_EQ(pair_3_count, 1);
}

TEST_F(LunaTypeTest, CanSaveUserdataAsRef)
{
    struct UserDefinedData
    {
        int x = 10;
        int y = 11;
        int z = 77;
    };

    namespace Luna = Gng2D::Luna;
    Luna::Type udd;
    {
        ScopedStack stack(L);
        auto*       mem = lua_newuserdata(L, sizeof(UserDefinedData));
        ASSERT_TRUE(lua_isuserdata(L, -1));
        new (mem) UserDefinedData{.x = 7, .y = -9, .z = 0};
        udd = stack.read(-1);
        ASSERT_TRUE(udd.isUserdata());
        ASSERT_EQ(udd.asUserdata().get(), mem);
    }
    ASSERT_EQ(lua_gettop(L), 0);
    UserDefinedData* data = (UserDefinedData*)udd.asUserdata().get();
    ASSERT_EQ(data->x, 7);
    ASSERT_EQ(data->y, -9);
    ASSERT_EQ(data->z, 0);
}
