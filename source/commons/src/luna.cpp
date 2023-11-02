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
                                               "\n-- END SCRIPT --");
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

Luna::Type Luna::readStack(int n)
{
    auto type = lua_type(L, n);
    switch (type)
    {
        case LUA_TNUMBER:
            if (lua_isinteger(L, n)) return Integer{lua_tointeger(L, n)};
            else return Float{lua_tonumber(L, n)};
        case LUA_TSTRING:
            return String{lua_tostring(L, n)};
        case LUA_TBOOLEAN:
            return bool{static_cast<bool>(lua_toboolean(L, n))};
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

