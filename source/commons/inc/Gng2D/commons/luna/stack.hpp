#pragma once
#include "Gng2D/commons/luna/type.hpp"

namespace Gng2D::Luna {
struct StackLock;
struct Stack
{
    Stack(lua_State* L)
        : L(L)
    {
    }
    Stack(const Stack&) = default;
    Stack(Stack&&)      = default;
    ~Stack()            = default;

    void pushNil();
    void pushInt(Integer);
    void pushFloat(Float);
    void pushString(const String&);
    void pushBool(Bool);
    void pushTable(const TableRef&);
    void pushFunction(const FunctionRef&);
    void pushUserdata(const UserdataRef&);

    void push(const Type&);
    void pushGlobal(const std::string&);

    void setMetaTable(int indx, const TableRef&);
    void pushMetaTable(int indx);

    Type read(int n) const;
    void setGlobal(const std::string&);
    TYPE is(int n) const;
    void pop(int n);
    int  top() const;

    void newTable();
    void setTableField(const Type& key, const Type& value, int tableIndx = -1);
    void setTableFieldFS(int tableIndx = -3);
    void pushTableField(const Type& key, int tableIndx = -1);
    void pushTableFieldFS(int tableIndx = -2);

    int callFunction(const FunctionRef&, TypeVector args = {});
    int callFunctionFS(int argsOnStack = 0);

    StackLock subscope();

    template <typename T, typename... Args>
    UserdataRef newUserdata(Args&&... args)
    {
        auto* mem = lua_newuserdata(L, sizeof(T));
        new (mem) T(std::forward<Args>(args)...);
        return read(-1).asUserdata();
    }

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
