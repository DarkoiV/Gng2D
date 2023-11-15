#include "Gng2D/commons/luna/stack.hpp"
#include "Gng2D/commons/luna/type.hpp"

using namespace Gng2D::Luna;

TableRef::TableRef(lua_State* state, int idx)
    : L(state)
{
    lua_pushvalue(L, idx);
    regRef = luaL_ref(L, LUA_REGISTRYINDEX);
    ptr    = lua_topointer(L, idx);
}

TableRef::~TableRef()
{
    luaL_unref(L, LUA_REGISTRYINDEX, regRef);
}

void TableRef::set(const Type& key, const Type& value)
{
    Stack::Lock lock(L);
    Stack       stack(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, regRef);
    stack.setTableField(key, value);
}

Type TableRef::get(const Type& key)
{
    Stack::Lock lock(L);
    Stack       stack(L);

    lua_rawgeti(L, LUA_REGISTRYINDEX, regRef);
    stack.pushTableField(key);
    return stack.read(-1);
}
