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

    Type                       read(const std::string&);
    std::optional<lua_Integer> readInt(const std::string&);
    std::optional<lua_Number>  readFloat(const std::string&);
    std::optional<std::string> readString(const std::string&);
    std::optional<bool>        readBool(const std::string&);
    std::optional<TableRef>    readTable(const std::string&);

    void createInt(const std::string&, lua_Integer);
    void createFloat(const std::string&, lua_Number);
    void createString(const std::string& name, const std::string& var);
    void createBool(const std::string&, bool);
    void createTable(const std::string&);

  private:
    lua_State* L{};

    void setEnv(const TableRef& env);
};
} // namespace Gng2D::Luna
