#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"
#include "Gng2D/commons/luna/stack.hpp"

using namespace Gng2D::Luna;

namespace {
template <typename>
struct tag
{
};

template <typename T, typename V>
struct get_index;

template <typename T, typename... Ts>
struct get_index<T, std::variant<Ts...>>
    : std::integral_constant<std::size_t, std::variant<tag<Ts>...>(tag<T>()).index()>
{
};
} // namespace

void Stack::pushNil()
{
    lua_pushnil(L);
}

void Stack::push(Integer value)
{
    lua_pushinteger(L, value);
}

void Stack::push(Float value)
{
    lua_pushnumber(L, value);
}

void Stack::push(const String& value)
{
    lua_pushstring(L, value.c_str());
}

void Stack::push(Bool value)
{
    lua_pushboolean(L, value);
}

void Stack::push(const TableRef& tr)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, tr.regRef);
}

void Stack::pushGlobal(const String& name)
{
    lua_getglobal(L, name.c_str());
}

void Stack::push(const Type& value)
{
    switch (value.index())
    {
    case 0:
        lua_pushnil(L);
        break;
    case 1:
        push(value.asInteger());
        break;
    case 2:
        push(value.asFloat());
        break;
    case 3:
        push(value.asString());
        break;
    case 4:
        push(value.asBool());
        break;
    case 5:
        push(value.asTable());
        break;
    [[unlikely]] default:
        LOG::ERROR("Fallthrough on push");
    }
}

Type Stack::read(int n)
{
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
        return TableRef(L, n);
    }

    LOG::DEBUG("Global is of non readable type");
    return Nil{};
}

TYPE Stack::is(int n)
{
    auto t = lua_type(L, n);
    switch (t)
    {
    case LUA_TNIL:
        return TYPE::NIL;
    case LUA_TNUMBER:
        if (lua_isinteger(L, n)) return TYPE::INT;
        else return TYPE::FLOAT;
    case LUA_TSTRING:
        return TYPE::STRING;
    case LUA_TBOOLEAN:
        return TYPE::BOOL;
    case LUA_TTABLE:
        return TYPE::TABLE;
    };

    LOG::DEBUG("Var is of non readable type");
    return TYPE::NIL;
}

void Stack::pop(int n)
{
    lua_pop(L, n);
}

void Stack::newTable()
{
    lua_newtable(L);
}

void Stack::setTableField(const Type& key, const Type& value)
{
    GNG2D_ASSERT(lua_istable(L, -1), "setttable requies table at index -1");
    push(key);
    push(value);
    lua_rawset(L, -2);
}

void Stack::pushTableField(const Type& key)
{
    GNG2D_ASSERT(lua_istable(L, -1), "setttable requies table at index -1");
    push(key);
    lua_rawget(L, -2);
}

StackLock::StackLock(lua_State* state)
    : L(state)
{
    top = lua_gettop(L);
}

StackLock::~StackLock()
{
    GNG2D_ASSERT(top <= lua_gettop(L), "Stack lock went out of scope!");
    lua_settop(L, top);
}
