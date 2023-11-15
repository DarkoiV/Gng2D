#include "Gng2D/commons/luna/type.hpp"

using namespace Gng2D::Luna;

TableRef::TableRef(lua_State* state, int idx)
    : L(state)
{
    lua_pushvalue(L, idx);
    regRef = luaL_ref(L, LUA_REGISTRYINDEX);
}

TableRef::~TableRef()
{
    luaL_unref(L, LUA_REGISTRYINDEX, regRef);
}
