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

void Stack::pushGlobal(const String& name)
{
    lua_getglobal(L, name.c_str());
}

void Stack::push(const Table& table)
{
    lua_createtable(L, 0, table.size());
    for (const auto& [k, v]: table)
    {
        switch (k.index())
        {
        case get_index<Integer, TableKey>():
            lua_pushinteger(L, std::get<Integer>(k));
            break;

        case get_index<Float, TableKey>():
            lua_pushnumber(L, std::get<Float>(k));
            break;

        case get_index<String, TableKey>():
            lua_pushstring(L, std::get<String>(k).c_str());
            break;
        }
        push(v);
        lua_rawset(L, -3);
    }
}

void Stack::push(const Type& value)
{
    switch (value.index())
    {
    case 0:
        lua_pushnil(L);
        break;
    case 1:
        lua_pushinteger(L, std::get<Integer>(value));
        break;
    case 2:
        lua_pushnumber(L, std::get<Float>(value));
        break;
    case 3:
        lua_pushstring(L, std::get<String>(value).c_str());
        break;
    case 4:
        lua_pushboolean(L, std::get<Bool>(value));
        break;
    case 5:
        push(std::get<Table>(value));
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
