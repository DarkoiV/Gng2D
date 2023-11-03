#include "Gng2D/commons/luna.hpp"
#include "Gng2D/commons/log.hpp"

using Gng2D::Luna;

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
                                               "\n -- END SCRIPT --");
}

void Luna::pushNil()
{
    lua_pushnil(L);
}

void Luna::pushInt(Luna::Integer value)
{
    lua_pushinteger(L, value);
}

void Luna::pushFloat(Luna::Float value)
{
    lua_pushnumber(L, value);
}

void Luna::pushString(const Luna::String& value)
{
    lua_pushstring(L, value.c_str());
}

void Luna::pushBool(Luna::Bool value)
{
    lua_pushboolean(L, value);
}

void Luna::pushGlobal(const String& name)
{
    lua_getglobal(L, name.c_str());
}

Luna::Type Luna::readStack(int n)
{
    GNG2D_ASSERT(lua_gettop(L) >= abs(n) and n != 0 , "Out of stack access");
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
            return std::make_unique<Table>(luaToTable(n));
    }
    LOG::DEBUG("Global is of non readable type");
    return Nil{};
}

void Luna::popStack(int n)
{
    lua_pop(L, n);
}

Luna::Type Luna::read(const std::string& name)
{
    StackLock lock(L);
    auto type = lua_getglobal(L, name.c_str());
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
            return std::make_unique<Table>(luaToTable(-1));
    }
    LOG::DEBUG("Global is of non readable type");
    return Nil{};
}

std::optional<lua_Integer> Luna::readInt(const std::string& name)
{
    StackLock lock(L);
    if (LUA_TNUMBER == lua_getglobal(L, name.c_str()))
    {
        int ret = lua_tointeger(L, -1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::DEBUG(name, "is not a number");
        return std::nullopt;
    }
}

std::optional<lua_Number> Luna::readFloat(const std::string& name)
{
    StackLock lock(L);
    if (LUA_TNUMBER == lua_getglobal(L, name.c_str()))
    {
        double ret = lua_tonumber(L, -1);
        return ret;
    }
    else [[unlikely]]
    {
        LOG::DEBUG(name, "is not a number");
        return std::nullopt;
    }
}

std::optional<std::string> Luna::readString(const std::string& name)
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

std::optional<bool> Luna::readBool(const std::string& name)
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

void Luna::createInt(const std::string& name, lua_Integer var)
{
    StackLock lock(L);
    lua_pushinteger(L, var);
    lua_setglobal(L, name.c_str());
}

void Luna::createFloat(const std::string& name, lua_Number var)
{
    StackLock lock(L);
    lua_pushnumber(L, var);
    lua_setglobal(L, name.c_str());
}

void Luna::createString(const std::string& name, const std::string& var)
{
    StackLock lock(L);
    lua_pushstring(L, var.c_str());
    lua_setglobal(L, name.c_str());
}

void Luna::createBool(const std::string& name, bool var)
{
    StackLock lock(L);
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

Luna::Table Luna::luaToTable(int n)
{
    StackLock lock(L);
    auto tableIndex = lua_gettop(L) + n + 1;
    Table result;

    pushNil();
    while(lua_next(L, tableIndex))
    {
        auto stackkey = readStack(-2);
        TableKey tkey;
        if (std::holds_alternative<Integer>(stackkey))
        {
            tkey = std::get<Integer>(stackkey);
        }
        else if(std::holds_alternative<String>(stackkey))
        {
            tkey = std::move(std::get<String>(stackkey));
        }
        else [[unlikely]]
        {
            LOG::WARN("Luna only supports indexes that are Integers or Strings, value will be ignored");
            popStack(1);
            continue;
        }

        result[tkey] = readStack(-1);
        popStack(1);
    }

    return result;
}

Luna::StackLock::StackLock(lua_State* L)
    : L(L)
{
    top = lua_gettop(L);
}

Luna::StackLock::~StackLock()
{
    GNG2D_ASSERT(top <= lua_gettop(L), "Stack lock went out of scope!");
    lua_settop(L, top);
}

