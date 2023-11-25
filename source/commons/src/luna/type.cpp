#include "Gng2D/commons/luna/type.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/luna/stack.hpp"

using namespace Gng2D::Luna;

Ref::Ref(lua_State* L, int idx)
    : L(L)
{
    lua_pushvalue(L, idx);
    ref = luaL_ref(L, LUA_REGISTRYINDEX);
}

Ref::~Ref()
{
    luaL_unref(L, LUA_REGISTRYINDEX, ref);
}

TableRef::TableRef(lua_State* state, int idx)
    : L(state)
{
    regRef = std::make_unique<Ref>(L, idx);
    ptr    = lua_topointer(L, idx);
}

TableRef::TableRef(const TableRef& from)
    : L(from.L)
{
    regRef = from.regRef;
    ptr    = from.ptr;
}

TableRef::TableRef(TableRef&& from)
    : L(from.L)
{
    regRef = std::move(from.regRef);
    ptr    = from.ptr;
}

TableRef& TableRef::operator=(const TableRef& from)
{
    GNG2D_ASSERT(L == from.L,
                 "Assigment operator for TableRef "
                 "only allowed for Refs belonging to the same Lua state");
    regRef = from.regRef;
    ptr    = from.ptr;
    return *this;
}

TableRef& TableRef::operator=(TableRef&& from)
{
    GNG2D_ASSERT(L == from.L,
                 "Assigment operator for TableRef "
                 "only allowed for Refs belonging to the same Lua state");
    regRef = std::move(from.regRef);
    ptr    = from.ptr;
    return *this;
}

TableRef::~TableRef() { }

void TableRef::set(const Type& key, const Type& value)
{
    ScopedStack stack(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, regRef->get());
    stack.setTableField(key, value);
}

Type TableRef::get(const Type& key) const
{
    ScopedStack stack(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, regRef->get());
    stack.pushTableField(key);
    return stack.read(-1);
}
