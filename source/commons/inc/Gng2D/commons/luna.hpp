#pragma once
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"
#include "lua.hpp"
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <variant>

namespace Gng2D
{
struct Luna 
{
    Luna();
    ~Luna();

    void doFile(const std::string& path, const std::string& env = "");
    void doString(const std::string& str, const std::string& env = "");

    using   Nil         = std::monostate;
    using   Integer     = lua_Integer;
    using   Float       = lua_Number;
    using   String      = std::string;
    using   Bool        = bool;
    struct  Table;
    using   TablePtr    = std::unique_ptr<Table>;
    using   TableKey    = std::variant<Integer, String>;
    using   Type        = std::variant<Nil, Integer, Float, String, Bool, TablePtr>;

    struct StackLock;
    void    pushNil();
    void    pushInt(Integer);
    void    pushFloat(Float);
    void    pushString(const String&);
    void    pushBool(Bool);
    Type    readStack(int n);
    void    popStack(int n);

    Type                        read(const std::string&);
    std::optional<lua_Integer>  readInt(const std::string&);
    std::optional<lua_Number>   readFloat(const std::string&);
    std::optional<std::string>  readString(const std::string&);
    std::optional<bool>         readBool(const std::string&);

    void createInt(const std::string&, lua_Integer);
    void createFloat(const std::string&, lua_Number);
    void createString(const std::string& name, const std::string& var);
    void createBool(const std::string&, bool);

    template<typename T>
    bool readToVar(const std::string&, T& var);

private:
    lua_State* L = luaL_newstate();

    void    setEnv(const std::string& env);
    Table   luaToTable(int n);

public:
    struct StackLock
    {
        StackLock(lua_State*);
        ~StackLock();

    private:
        lua_State*  L;
        int         top;
    };

    struct Table : std::map<TableKey, Type>
    {
        using std::map<TableKey, Type>::map;
    };
};

template<typename T>
bool Luna::readToVar(const std::string& name, T& var)
{
    if constexpr (std::is_integral_v<T> and not std::is_same_v<T, bool>)
    {
        if (auto value = readInt(name); value) var = *value;
        else return false;
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        if (auto value = readFloat(name); value) var = *value;
        else return false;
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        if (auto value = readString(name); value) var = std::move(*value);
        else return false;
    }
    else if constexpr (std::is_same_v<T, bool>)
    {
        if (auto value = readBool(name); value) var = *value;
        else return false;
    }
    else GNG2D_ASSERT_CONSTEXPR("Type not supported");
    LOG::DEBUG(name, "=", var);
    return true;
}
}
