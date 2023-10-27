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

Luna::~Luna()
{
    lua_close(L);
}

void Luna::doFile(const std::string& path, const std::string& env)
{
    luaL_loadfile(L, path.c_str());
    if (not env.empty()) setEnv(env);
    if (lua_pcall(L, 0, 0, 0) != 0) LOG::ERROR("Issue running dofile", path);
}

void Luna::doString(const std::string& str, const std::string& env)
{
    luaL_loadstring(L, str.c_str());
    if (not env.empty()) setEnv(env);
    if (lua_pcall(L, 0, 0, 0) != 0) LOG::ERROR("Issue running script: \n", 
                                               "-- LUA SCRIPT --\n", 
                                               str, 
                                               "\n-- END SCRIPT --");
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
        LOG::WARN(name, "is not a number");
        return std::nullopt;
    }
}

std::optional<double> Luna::readGlobalFloat(const std::string& name)
{
    LuaStackLock lock(L);
    if (LUA_TNUMBER == lua_getglobal(L, name.c_str()))
    {
        double ret = lua_tonumber(L, -1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::WARN(name, "is not a number");
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
        LOG::WARN(name, "is not a string");
        return std::nullopt;
    }
}

std::optional<bool> Luna::readGlobalBool(const std::string& name)
{
    LuaStackLock lock(L);
    if (LUA_TBOOLEAN == lua_getglobal(L, name.c_str()))
    {
        bool ret = lua_toboolean(L, -1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::WARN(name, "is not a bool");
        return std::nullopt;
    }
}

void Luna::createGlobalInt(const std::string& name, int var)
{
    LuaStackLock lock(L);
    lua_pushinteger(L, var);
    lua_setglobal(L, name.c_str());
}

void Luna::createGlobalFloat(const std::string& name, double var)
{
    LuaStackLock lock(L);
    lua_pushnumber(L, var);
    lua_setglobal(L, name.c_str());
}

void Luna::createGlobalString(const std::string& name, const std::string& var)
{
    LuaStackLock lock(L);
    lua_pushstring(L, var.c_str());
    lua_setglobal(L, name.c_str());
}

void Luna::createGlobalBool(const std::string& name, bool var)
{
    LuaStackLock lock(L);
    lua_pushboolean(L, var);
    lua_setglobal(L, name.c_str());
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

