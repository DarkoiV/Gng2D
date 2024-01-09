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

// TABLE REF ///////////////////////////////////////////////////////////////////

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

void TableRef::createSubTable(const Type& key)
{
    ScopedStack stack(L);
    stack.push(*this);
    stack.push(key);
    stack.newTable();
    stack.setTableFieldFS();
}

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

void TableRef::setMetaTable(const TableRef& table)
{
    ScopedStack stack(L);
    stack.push(*this);
    stack.setMetaTable(-1, table);
}

TableRef TableRef::getMetaTable()
{
    ScopedStack stack(L);
    stack.push(*this);
    stack.pushMetaTable(-1);
    return stack.read(-1).asTable();
}

TableRef::Iterator::Iterator(lua_State* L, const TableRef& t)
    : tableRef(t)
{
    thread    = lua_newthread(L);
    threadRef = std::make_unique<Ref>(L, -1);
    lua_pop(L, 1); // Pop thread

    Stack stack(thread);
    stack.push(t);
    stack.pushNil();
    isThereNextElement = lua_next(thread, -2);
};

TableRef::Iterator::Iterator(const TableRef& t)
    : tableRef(t)
{
    isThereNextElement = false;
}

TableRef::Iterator::Iterator(const Iterator& from)
    : tableRef(from.tableRef)
{
    thread    = lua_newthread(from.thread);
    threadRef = std::make_unique<Ref>(from.thread, -1);
    lua_pop(from.thread, 1); // Pop thread

    Stack stack(thread);
    Stack fromStack(from.thread);
    for (int i = 1; i <= fromStack.top(); i++)
    {
        stack.push(fromStack.read(i));
    }
    isThereNextElement = from.isThereNextElement;
}

TableRef::Iterator::Iterator(Iterator&& from)
    : tableRef(from.tableRef)
{
    thread             = from.thread;
    from.thread        = nullptr;
    threadRef          = from.threadRef;
    isThereNextElement = from.isThereNextElement;
}

TableRef::Iterator& TableRef::Iterator::operator++()
{
    GNG2D_ASSERT(isThereNextElement);
    GNG2D_ASSERT(lua_gettop(thread) == 3, "actual:", lua_gettop(thread));
    lua_pop(thread, 1);
    isThereNextElement = lua_next(thread, -2);
    return *this;
}

TableRef::Iterator TableRef::Iterator::operator++(int)
{
    GNG2D_ASSERT(isThereNextElement);
    GNG2D_ASSERT(lua_gettop(thread) == 3, "actual:", lua_gettop(thread));
    Iterator tmp = *this;
    ++(*this);
    return tmp;
}

TableRef::Iterator::value_type TableRef::Iterator::operator*()
{
    Stack stack(thread);
    return {stack.read(-2), stack.read(-1)};
}

bool TableRef::Iterator::areKeysSame(const Iterator& lhs, const Iterator& rhs)
{
    return Stack(lhs.thread).read(-2) == Stack(rhs.thread).read(-2);
}

// FUNCTION REF ////////////////////////////////////////////////////////////////

FunctionRef::FunctionRef(lua_State* state, int idx)
    : L(state)
{
    regRef = std::make_unique<Ref>(L, idx);
    ptr    = lua_topointer(L, idx);
}

FunctionRef::FunctionRef(const FunctionRef& from)
    : L(from.L)
{
    regRef = from.regRef;
    ptr    = from.ptr;
}

FunctionRef::FunctionRef(FunctionRef&& from)
    : L(from.L)
{
    regRef = std::move(from.regRef);
    ptr    = from.ptr;
}

FunctionRef& FunctionRef::operator=(const FunctionRef& from)
{
    GNG2D_ASSERT(L == from.L,
                 "Assigment operator for FunctionRef "
                 "only allowed for Refs belonging to the same Lua state");
    regRef = from.regRef;
    ptr    = from.ptr;
    return *this;
}

FunctionRef& FunctionRef::operator=(FunctionRef&& from)
{
    GNG2D_ASSERT(L == from.L,
                 "Assigment operator for FunctionRef "
                 "only allowed for Refs belonging to the same Lua state");
    regRef = std::move(from.regRef);
    ptr    = from.ptr;
    return *this;
}

// USERDATA REF ////////////////////////////////////////////////////////////////

UserdataRef::UserdataRef(lua_State* state, int idx)
    : L(state)
{
    regRef = std::make_unique<Ref>(L, idx);
    value  = lua_touserdata(L, idx);
}

UserdataRef::UserdataRef(const UserdataRef& from)
    : L(from.L)
{
    regRef = from.regRef;
    value  = from.value;
}

UserdataRef::UserdataRef(UserdataRef&& from)
    : L(from.L)
{
    regRef = std::move(from.regRef);
    value  = from.value;
}

UserdataRef& UserdataRef::operator=(const UserdataRef& from)
{
    GNG2D_ASSERT(L == from.L,
                 "Assigment operator for UserdataRef "
                 "only allowed for Refs belonging to the same Lua state");
    regRef = from.regRef;
    value  = from.value;
    return *this;
}

UserdataRef& UserdataRef::operator=(UserdataRef&& from)
{
    GNG2D_ASSERT(L == from.L,
                 "Assigment operator for UserdataRef "
                 "only allowed for Refs belonging to the same Lua state");
    regRef = std::move(from.regRef);
    value  = from.value;
    return *this;
}

void UserdataRef::setMetaTable(const TableRef& table)
{
    ScopedStack stack(L);
    stack.push(*this);
    stack.setMetaTable(-1, table);
}

TableRef UserdataRef::getMetaTable()
{
    ScopedStack stack(L);
    stack.push(*this);
    stack.pushMetaTable(-1);
    return stack.read(-1).asTable();
}
