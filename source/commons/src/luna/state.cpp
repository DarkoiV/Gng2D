#include "Gng2D/commons/luna/state.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/luna/stack.hpp"

using namespace Gng2D::Luna;

State::State()
    : L(luaL_newstate())
{
    if (not L) LOG::FATAL("Failed to create lua state");
}

State::~State()
{
    lua_close(L);
}

void State::doFile(const std::string& path)
{
    GNG2D_ASSERT(luaL_dofile(L, path.c_str()) == LUA_OK, lua_tostring(L, -1));
}

void State::doFile(const std::string& path, const TableRef& env)
{
    if (luaL_loadfile(L, path.c_str()) != LUA_OK) LOG::ERROR(lua_tostring(L, -1));
    setEnv(env);
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) LOG::ERROR(lua_tostring(L, -1));
}

void State::doString(const std::string& str, const TableRef& env)
{
    if (luaL_loadstring(L, str.c_str()) != LUA_OK) LOG::ERROR(lua_tostring(L, -1));
    setEnv(env);
    if (lua_pcall(L, 0, 0, 0) != LUA_OK) LOG::ERROR(lua_tostring(L, -1));
}

void State::doString(const std::string& str)
{
    GNG2D_ASSERT(luaL_dostring(L, str.c_str()) == LUA_OK, lua_tostring(L, -1));
}

ScopedStack State::getStack()
{
    return ScopedStack(L);
}

Type State::read(const std::string& name) const
{
    ScopedStack stack(L);
    auto        type = lua_getglobal(L, name.c_str());
    return stack.read(-1);
}

std::optional<lua_Integer> State::readInt(const std::string& name) const
{
    StackLock lock(L);
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

std::optional<lua_Number> State::readFloat(const std::string& name) const
{
    StackLock lock(L);
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

std::optional<std::string> State::readString(const std::string& name) const
{
    StackLock lock(L);
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

std::optional<bool> State::readBool(const std::string& name) const
{
    StackLock lock(L);
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

std::optional<TableRef> State::readTable(const std::string& name) const
{
    ScopedStack stack(L);
    if (LUA_TTABLE == lua_getglobal(L, name.c_str()))
    {
        return TableRef(L, -1);
    }
    else [[unlikely]]
    {
        LOG::DEBUG(name, "is not a table");
        return std::nullopt;
    }
}

std::optional<FunctionRef> State::readFunction(const std::string& name) const
{
    ScopedStack stack(L);
    if (LUA_TFUNCTION == lua_getglobal(L, name.c_str()))
    {
        return FunctionRef(L, -1);
    }
    else [[unlikely]]
    {
        LOG::DEBUG(name, "is not a table");
        return std::nullopt;
    }
}

std::optional<UserdataRef> State::readUserdata(const std::string& name) const
{
    ScopedStack stack(L);
    if (LUA_TUSERDATA == lua_getglobal(L, name.c_str()))
    {
        return UserdataRef(L, -1);
    }
    else [[unlikely]]
    {
        LOG::DEBUG(name, "is not a table");
        return std::nullopt;
    }
}

void State::createInt(const std::string& name, lua_Integer var)
{
    StackLock lock(L);
    lua_pushinteger(L, var);
    lua_setglobal(L, name.c_str());
}

void State::createFloat(const std::string& name, lua_Number var)
{
    StackLock lock(L);
    lua_pushnumber(L, var);
    lua_setglobal(L, name.c_str());
}

void State::createString(const std::string& name, const std::string& var)
{
    StackLock lock(L);
    lua_pushstring(L, var.c_str());
    lua_setglobal(L, name.c_str());
}

void State::createBool(const std::string& name, bool var)
{
    StackLock lock(L);
    lua_pushboolean(L, var);
    lua_setglobal(L, name.c_str());
}

void State::createTable(const std::string& name)
{
    StackLock lock(L);
    lua_newtable(L);
    lua_setglobal(L, name.c_str());
}

void State::setEnv(const TableRef& env)
{
    GNG2D_ASSERT(lua_isfunction(L, -1), "setEnv requires function at -1 index");
    ScopedStack stack(L);
    stack.push(env);
    GNG2D_ASSERT(std::string("_ENV") == lua_setupvalue(L, -2, 1), "setupvalue did not set env");
}
