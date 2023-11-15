#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/luna/state.hpp"

using namespace Gng2D::Luna;

State::State()
    : L(luaL_newstate())
    , stack(L)
{
    if (not L) LOG::FATAL("Failed to create lua state");
}

State::~State()
{
    lua_close(L);
}

void State::doFile(const std::string& path, const std::string& env)
{
    if (luaL_loadfile(L, path.c_str()) != 0) LOG::ERROR(lua_tostring(L, 1));
    if (not env.empty()) setEnv(env);
    if (lua_pcall(L, 0, 0, 0) != 0) LOG::ERROR(lua_tostring(L, -1));
}

void State::doString(const std::string& str, const std::string& env)
{
    if (luaL_loadstring(L, str.c_str()) != 0) LOG::ERROR(lua_tostring(L, -1));
    if (not env.empty()) setEnv(env);
    if (lua_pcall(L, 0, 0, 0) != 0) LOG::ERROR(lua_tostring(L, -1));
}

Type State::read(const std::string& name)
{
    Stack::Lock lock(L);
    auto        type = lua_getglobal(L, name.c_str());
    switch (type)
    {
    case LUA_TNIL:
        return Nil{};
    case LUA_TNUMBER:
        if (lua_isinteger(L, -1)) return Integer{lua_tointeger(L, -1)};
        else return Float{lua_tonumber(L, -1)};
    case LUA_TSTRING:
        return String{lua_tostring(L, -1)};
    case LUA_TBOOLEAN:
        return bool{static_cast<bool>(lua_toboolean(L, -1))};
    }
    LOG::DEBUG("Global is of non readable type");
    return Nil{};
}

std::optional<lua_Integer> State::readInt(const std::string& name)
{
    Stack::Lock lock(L);
    if (LUA_TNUMBER == lua_getglobal(L, name.c_str()) and lua_isinteger(L, -1))
    {
        lua_Integer ret = lua_tointeger(L, -1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::DEBUG(name, "is not a integer");
        return std::nullopt;
    }
}

std::optional<lua_Number> State::readFloat(const std::string& name)
{
    Stack::Lock lock(L);
    if (LUA_TNUMBER == lua_getglobal(L, name.c_str()))
    {
        lua_Number ret = lua_tonumber(L, -1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::DEBUG(name, "is not a number");
        return std::nullopt;
    }
}

std::optional<std::string> State::readString(const std::string& name)
{
    Stack::Lock lock(L);
    if (LUA_TSTRING == lua_getglobal(L, name.c_str()))
    {
        std::string ret{lua_tostring(L, -1)};
        return ret;
    }
    else [[unlikely]]
    {
        LOG::DEBUG(name, "is not a string");
        return std::nullopt;
    }
}

std::optional<bool> State::readBool(const std::string& name)
{
    Stack::Lock lock(L);
    if (LUA_TBOOLEAN == lua_getglobal(L, name.c_str()))
    {
        bool ret = lua_toboolean(L, -1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::DEBUG(name, "is not a bool");
        return std::nullopt;
    }
}

void State::createInt(const std::string& name, lua_Integer var)
{
    Stack::Lock lock(L);
    lua_pushinteger(L, var);
    lua_setglobal(L, name.c_str());
}

void State::createFloat(const std::string& name, lua_Number var)
{
    Stack::Lock lock(L);
    lua_pushnumber(L, var);
    lua_setglobal(L, name.c_str());
}

void State::createString(const std::string& name, const std::string& var)
{
    Stack::Lock lock(L);
    lua_pushstring(L, var.c_str());
    lua_setglobal(L, name.c_str());
}

void State::createBool(const std::string& name, bool var)
{
    Stack::Lock lock(L);
    lua_pushboolean(L, var);
    lua_setglobal(L, name.c_str());
}

void State::setEnv(const std::string& env)
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
