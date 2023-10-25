#include "Gng2D/core/luna.hpp"
#include "Gng2D/core/log.hpp"

using Gng2D::Luna;

Luna::Luna()
{
    if (not L) LOG::FATAL("Failed to create lua state");
}

void Luna::doFile(const std::string& filePath)
{
    luaL_dofile(L, filePath.c_str());
}

std::optional<int> Luna::readGlobalInt(const std::string& name)
{
    if (LUA_TNUMBER == lua_getglobal(L, name.c_str()))
    {
        int ret = lua_tointeger(L, -1);
        lua_pop(L, 1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::ERROR(name, "is not a number");
        lua_pop(L, 1);
        return std::nullopt;
    }
}

std::optional<std::string> Luna::readGlobalString(const std::string& name)
{
    if (LUA_TSTRING == lua_getglobal(L, name.c_str()))
    {
        std::string ret{lua_tostring(L, -1)};
        lua_pop(L, 1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::ERROR(name, "is not a string");
        lua_pop(L, 1);
        return std::nullopt;
    }
}

