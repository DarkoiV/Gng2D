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

    template <int (*F)(Stack)>
    void registerFunction(const std::string& name)
    {
        auto* fn = +[](lua_State* L) { return F(Stack(L)); };
        lua_register(L, name.c_str(), fn);
    }

    template <auto Method>
        requires(std::is_member_function_pointer_v<decltype(Method)>)
    void registerMethod(auto& obj, const std::string& name)
    {
        using ObjType = std::remove_reference_t<decltype(obj)>;
        static_assert(requires {
            {
                (obj.*Method)(Stack(L))
            } -> std::same_as<int>;
        });

        auto* fn = +[](lua_State* L) -> int
        {
            auto* objPtr = (ObjType*)lua_touserdata(L, lua_upvalueindex(1));
            GNG2D_ASSERT(objPtr);
            return (objPtr->*Method)(Stack(L));
        };
        lua_pushlightuserdata(L, &obj);
        lua_pushcclosure(L, fn, 1);
        lua_setglobal(L, name.c_str());
    }

  private:
    lua_State* L{};

    void setEnv(const TableRef& env);
};
} // namespace Gng2D::Luna
