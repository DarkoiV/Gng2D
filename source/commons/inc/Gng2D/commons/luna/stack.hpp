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

    struct Lock;
    void pushNil();
    void push(Integer);
    void push(Float);
    void push(const String&);
    void push(Bool);
    void push(const TableRef&);

    void push(const Type&);
    void pushGlobal(const String&);

    Type read(int n);
    void pop(int n);

  private:
    lua_State* L;
};

struct Stack::Lock
{
    Lock(lua_State*);
    ~Lock();

  private:
    lua_State* L;
    int        top;
};
} // namespace Gng2D::Luna
