#include "Gng2D/core/luna.hpp"
#include "Gng2D/core/log.hpp"

using Gng2D::Luna;

namespace 
{
struct LuaStackLock 
{
    LuaStackLock(lua_State* L)
        : L(L)
    {
        top = lua_gettop(L);
    }

    ~LuaStackLock()
    {
        lua_settop(L, top);
    }
private:
    lua_State*  L;
    int         top;
};
}

Luna::Luna()
{
    if (not L) LOG::FATAL("Failed to create lua state");
}

void Luna::doFile(const std::string& path, const std::string& env)
{
    luaL_loadfile(L, path.c_str());
    if (not env.empty()) setEnv(env);
    lua_pcall(L, 0, 0, 0);
}

std::optional<int> Luna::readGlobalInt(const std::string& name)
{
    LuaStackLock lock(L);
    if (LUA_TNUMBER == lua_getglobal(L, name.c_str()))
    {
        int ret = lua_tointeger(L, -1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::ERROR(name, "is not a number");
        return std::nullopt;
    }
}

std::optional<std::string> Luna::readGlobalString(const std::string& name)
{
    LuaStackLock lock(L);
    if (LUA_TSTRING == lua_getglobal(L, name.c_str()))
    {
        std::string ret{lua_tostring(L, -1)};
        return ret;
    }
    else [[unlikely]]
    {
        LOG::ERROR(name, "is not a string");
        return std::nullopt;
    }
}

void Luna::setEnv(const std::string& env)
{
    auto envType = lua_getglobal(L, env.c_str());
    if (envType != LUA_TTABLE and envType != LUA_TNIL) [[unlikely]] 
    {
        return LOG::ERROR("Cannot create env, name:", env, "is used by another variable");
    }
    if (envType == LUA_TNIL)
    {
        LOG::INFO("Creating new env:", env);
        lua_pop(L, 1);
        lua_newtable(L);
        lua_setglobal(L, env.c_str());
        lua_getglobal(L, env.c_str());
    }
    lua_setupvalue(L, -2, 1);
}

