#include "Gng2D/commons/luna_table.hpp"
#include "Gng2D/commons/assert.hpp"

using Gng2D::LunaTable;

std::optional<LunaTable> LunaTable::load(lua_State* pL, const std::string& name)
{
    L = lua_newthread(pL);
    auto type = lua_getglobal(L, name.c_str());
    if (type != LUA_TTABLE) [[unlikely]]
    {
        auto res = lua_closethread(L, NULL);
        GNG2D_ASSERT(res == LUA_OK, "Failed to close lua thread");
        LOG::DEBUG(name, "is not a table");
        return std::nullopt;
    }

    return LunaTable{L};
}

LunaTable::~LunaTable()
{
    auto res = lua_closethread(L, NULL);
    GNG2D_ASSERT(res == LUA_OK, "Failed to close lua thread");
}

