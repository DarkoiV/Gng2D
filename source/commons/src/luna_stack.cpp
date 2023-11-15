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

Stack::Lock::Lock(lua_State* state)
    : L(state)
{
    top = lua_gettop(L);
}

Stack::Lock::~Lock()
{
    GNG2D_ASSERT(top <= lua_gettop(L), "Stack lock went out of scope!");
    lua_settop(L, top);
}
