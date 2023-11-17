#include "Gng2D/commons/luna/type.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/luna/stack.hpp"

using namespace Gng2D::Luna;

TableRef::TableRef(lua_State* state, int idx)
    : L(state)
{
    lua_pushvalue(L, idx);
    regRef = luaL_ref(L, LUA_REGISTRYINDEX);
    ptr    = lua_topointer(L, idx);
}

TableRef::TableRef(const TableRef& from)
    : L(from.L)
{
    lua_rawgeti(L, LUA_REGISTRYINDEX, from.regRef);
    regRef = luaL_ref(L, LUA_REGISTRYINDEX);
    ptr    = from.ptr;
}

TableRef::TableRef(TableRef&& from)
    : L(from.L)
{
    regRef = from.regRef;
    ptr    = from.ptr;
    from.markAsMovedFrom();
}

TableRef& TableRef::operator=(const TableRef& from)
{
    GNG2D_ASSERT(L == from.L,
                 "Assigment operator for TableRef "
                 "only allowed for Refs belonging to the same Lua state");
    lua_rawgeti(L, LUA_REGISTRYINDEX, from.regRef);
    regRef = luaL_ref(L, LUA_REGISTRYINDEX);
    ptr    = from.ptr;
    return *this;
}

TableRef& TableRef::operator=(TableRef&& from)
{
    GNG2D_ASSERT(L == from.L,
                 "Assigment operator for TableRef "
                 "only allowed for Refs belonging to the same Lua state");
    regRef = from.regRef;
    ptr    = from.ptr;
    from.markAsMovedFrom();
    return *this;
}

TableRef::~TableRef()
{
    if (ownsRef()) luaL_unref(L, LUA_REGISTRYINDEX, regRef);
}

void TableRef::set(const Type& key, const Type& value)
{
    ScopedStack stack(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, regRef);
    stack.setTableField(key, value);
}

Type TableRef::get(const Type& key)
{
    ScopedStack stack(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, regRef);
    stack.pushTableField(key);
    return stack.read(-1);
}

void TableRef::markAsMovedFrom()
{
    ptr = nullptr;
}

bool TableRef::ownsRef()
{
    return ptr != nullptr;
}
