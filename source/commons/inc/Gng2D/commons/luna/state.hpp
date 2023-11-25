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

    void createInt(const std::string&, lua_Integer);
    void createFloat(const std::string&, lua_Number);
    void createString(const std::string& name, const std::string& var);
    void createBool(const std::string&, bool);
    void createTable(const std::string&);

    template <int (*F)(Stack)>
    void registerFunction(const std::string& name)
    {
        auto* fn = +[](lua_State* L) { return F(Stack(L)); };
        lua_register(L, name.c_str(), fn);
    }

  private:
    lua_State* L{};

    void setEnv(const TableRef& env);
};
} // namespace Gng2D::Luna
