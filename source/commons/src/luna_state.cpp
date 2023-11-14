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

Type State::readStack(int n)
{
    GNG2D_ASSERT(lua_gettop(L) >= abs(n) and n != 0, "Out of stack access");
    auto type = lua_type(L, n);
    switch (type)
    {
    case LUA_TNIL:
        return Nil{};
    case LUA_TNUMBER:
        if (lua_isinteger(L, n)) return Integer{lua_tointeger(L, n)};
        else return Float{lua_tonumber(L, n)};
    case LUA_TSTRING:
        return String{lua_tostring(L, n)};
    case LUA_TBOOLEAN:
        return bool{static_cast<bool>(lua_toboolean(L, n))};
    case LUA_TTABLE:
        return luaToTable(n);
    }
    LOG::DEBUG("Global is of non readable type");
    return Nil{};
}

void State::popStack(int n)
{
    lua_pop(L, n);
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
    case LUA_TTABLE:
        return luaToTable(-1);
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

std::optional<Table> State::readTable(const std::string& name)
{
    Stack::Lock lock(L);
    if (LUA_TTABLE == lua_getglobal(L, name.c_str()))
    {
        return luaToTable(-1);
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

void State::createTable(const std::string& name, const Table& table)
{
    Stack::Lock lock(L);
    stack.push(table);
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

Table State::luaToTable(int n)
{
    Stack::Lock lock(L);
    auto        tableIndex = lua_gettop(L) + n + 1;
    Table       result;

    stack.pushNil();
    while (lua_next(L, tableIndex))
    {
        auto     stackkey = readStack(-2);
        TableKey tkey;
        if (std::holds_alternative<Integer>(stackkey))
        {
            tkey = std::get<Integer>(stackkey);
        }
        else if (std::holds_alternative<Float>(stackkey))
        {
            tkey = std::get<Float>(stackkey);
        }
        else if (std::holds_alternative<String>(stackkey))
        {
            tkey = std::move(std::get<String>(stackkey));
        }
        else [[unlikely]]
        {
            LOG::WARN(
                "Luna only supports indexes that are Integers or Strings, value will be ignored");
            popStack(1);
            continue;
        }

        auto val = readStack(-1);
        if (not val.isNil()) result[tkey] = val;
        popStack(1);
    }

    return result;
}
