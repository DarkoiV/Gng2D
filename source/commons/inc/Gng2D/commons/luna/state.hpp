#pragma once
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

    void  doFile(const std::string& path, const std::string& env = "");
    void  doString(const std::string& str, const std::string& env = "");
    Table readFileAsTable(const std::string& path);

    struct StackLock;
    void pushNil();
    void pushInt(Integer);
    void pushFloat(Float);
    void pushString(const String&);
    void pushBool(Bool);
    void pushTable(const Table&);
    void push(const Type&);
    void pushGlobal(const String&);
    Type readStack(int n);
    void popStack(int n);

    Type                       read(const std::string&);
    std::optional<lua_Integer> readInt(const std::string&);
    std::optional<lua_Number>  readFloat(const std::string&);
    std::optional<std::string> readString(const std::string&);
    std::optional<bool>        readBool(const std::string&);
    std::optional<Table>       readTable(const std::string&);

    void createInt(const std::string&, lua_Integer);
    void createFloat(const std::string&, lua_Number);
    void createString(const std::string& name, const std::string& var);
    void createBool(const std::string&, bool);
    void createTable(const std::string&, const Table&);

  private:
    lua_State*             L{};
    const uint32_t         LThreadNo{};
    inline static uint32_t LThreads;

    void  setEnv(const std::string& env);
    Table luaToTable(int n);

  public:
    struct StackLock
    {
        StackLock(lua_State*);
        ~StackLock();

      private:
        lua_State* L;
        int        top;
    };
};
} // namespace Gng2D::Luna
