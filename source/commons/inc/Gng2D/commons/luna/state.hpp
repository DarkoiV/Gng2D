#pragma once
#include "Gng2D/commons/luna/stack.hpp"
#include "Gng2D/commons/luna/type.hpp"
#include "lua.hpp"
#include <optional>
#include <string>

namespace Gng2D::Luna {
struct State
{
    State();
    State(const State&) = delete;
    State(State&&)      = delete;
    ~State();

    void doFile(const std::string& path);
    void doFile(const std::string& path, const TableRef& env);
    void doString(const std::string& str);
    void doString(const std::string& str, const TableRef& env);

    ScopedStack getStack();

    Type                       read(const std::string&) const;
    std::optional<lua_Integer> readInt(const std::string&) const;
    std::optional<lua_Number>  readFloat(const std::string&) const;
    std::optional<std::string> readString(const std::string&) const;
    std::optional<bool>        readBool(const std::string&) const;
    std::optional<TableRef>    readTable(const std::string&) const;
    std::optional<FunctionRef> readFunction(const std::string&) const;
    std::optional<UserdataRef> readUserdata(const std::string&) const;

    void     createInt(const std::string&, lua_Integer);
    void     createFloat(const std::string&, lua_Number);
    void     createString(const std::string& name, const std::string& var);
    void     createBool(const std::string&, bool);
    void     createTable(const std::string&);
    TableRef createTableRef();

    template <int (*F)(Stack, TypeVector)>
    void registerFunction(const std::string& name, std::optional<TableRef> env = std::nullopt)
    {
        auto* fn = +[](lua_State* L)
        {
            Stack      stack(L);
            TypeVector args;
            for (int i = 1; i <= stack.top(); i++)
            {
                args.emplace_back(stack.read(i));
            }
            return F(std::move(stack), std::move(args));
        };

        if (not env) lua_register(L, name.c_str(), fn);
        else
        {
            auto stack = getStack();
            stack.push(*env);
            stack.push(name);
            lua_pushcfunction(L, fn);
            stack.setTableFieldFS();
        }
    }

    template <auto Method>
        requires(std::is_member_function_pointer_v<decltype(Method)>)
    void
    registerMethod(auto& obj, const std::string& name, std::optional<TableRef> env = std::nullopt)
    {
        using ObjType = std::remove_reference_t<decltype(obj)>;
        static_assert(requires {
            {
                (obj.*Method)(Stack(L), TypeVector())
            } -> std::same_as<int>;
        });

        auto* fn = +[](lua_State* L) -> int
        {
            auto* objPtr = (ObjType*)lua_touserdata(L, lua_upvalueindex(1));
            GNG2D_ASSERT(objPtr);
            Stack      stack(L);
            TypeVector args;
            for (int i = 1; i <= stack.top(); i++)
            {
                args.emplace_back(stack.read(i));
            }
            return (objPtr->*Method)(std::move(stack), std::move(args));
        };

        if (not env)
        {
            lua_pushlightuserdata(L, &obj);
            lua_pushcclosure(L, fn, 1);
            lua_setglobal(L, name.c_str());
        }
        else
        {
            auto stack = getStack();
            stack.push(*env);
            stack.push(name);
            lua_pushlightuserdata(L, &obj);
            lua_pushcclosure(L, fn, 1);
            stack.setTableFieldFS();
        }
    }

  private:
    lua_State* L{};

    void setEnv(const TableRef& env);
};
} // namespace Gng2D::Luna
