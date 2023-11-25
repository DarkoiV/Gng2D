#pragma once
#include "Gng2D/commons/luna/type.hpp"

namespace Gng2D::Luna {
struct Stack
{
    Stack(lua_State* L)
        : L(L)
    {
    }
    Stack(const Stack&) = delete;
    Stack(Stack&&)      = delete;
    ~Stack()            = default;

    void pushNil();
    void pushInt(Integer);
    void pushFloat(Float);
    void pushString(const String&);
    void pushBool(Bool);
    void pushTable(const TableRef&);

    void push(const Type&);
    void pushGlobal(const String&);

    Type read(int n) const;
    TYPE is(int n) const;
    void pop(int n);
    int  top() const;

    void newTable();
    void setTableField(const Type& key, const Type& value, int tableIndx = -1);
    void setTableFieldFS(int tableIndx = -3);
    void pushTableField(const Type& key, int tableIndx = -1);
    void pushTableFieldFS(int tableIndx = -2);

  private:
    lua_State* L;
};

struct StackLock
{
    StackLock(lua_State*);
    ~StackLock();

  private:
    lua_State* L;
    int        top_;
};

struct ScopedStack
    : Stack
    , StackLock
{
    ScopedStack(lua_State* s)
        : Stack(s)
        , StackLock(s)
    {
    }
};

} // namespace Gng2D::Luna
