#pragma once
#include "lua.hpp"
#include <map>
#include <optional>
#include <string>
#include <variant>

namespace Gng2D {
struct Luna
{
    Luna();
    Luna(const Luna&) = delete;
    Luna(Luna&&)      = delete;
    ~Luna();

    using Nil     = std::monostate;
    using Integer = lua_Integer;
    using Float   = lua_Number;
    using String  = std::string;
    using Bool    = bool;
    struct Table;
    using TableKey = std::variant<Integer, Float, String>;
    struct Type;

    void  doFile(const std::string& path, const std::string& env = "");
    void  doString(const std::string& str, const std::string& env = "");
    Table readFileAsTable(const std::string& path);

    template <typename T>
    constexpr static bool is(const Type&);

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

    struct Table : std::map<TableKey, Type>
    {
        using Base = std::map<TableKey, Type>;
        using Base::Base;
    };

    struct Type : std::variant<Nil, Integer, Float, String, Bool, Table>
    {
        using std::variant<Nil, Integer, Float, String, Bool, Table>::variant;

        constexpr bool isNil() const { return std::holds_alternative<Nil>(*this); }
        constexpr bool isInteger() const { return std::holds_alternative<Integer>(*this); }
        constexpr bool isFloat() const { return std::holds_alternative<Float>(*this); }
        constexpr bool isString() const { return std::holds_alternative<String>(*this); }
        constexpr bool isBool() const { return std::holds_alternative<Bool>(*this); }
        constexpr bool isTable() const { return std::holds_alternative<Table>(*this); }
        auto&          asInteger() { return std::get<Integer>(*this); };
        auto&          asFloat() { return std::get<Float>(*this); };
        auto&          asString() { return std::get<String>(*this); };
        auto&          asBool() { return std::get<Bool>(*this); };
        auto&          asTable() { return std::get<Table>(*this); };
        const auto&    asInteger() const { return std::get<Integer>(*this); };
        const auto&    asFloat() const { return std::get<Float>(*this); };
        const auto&    asString() const { return std::get<String>(*this); };
        const auto&    asBool() const { return std::get<Bool>(*this); };
        const auto&    asTable() const { return std::get<Table>(*this); };
    };
};

template <typename T>
constexpr bool Luna::is(const Luna::Type& t)
{
    return std::holds_alternative<T>(t);
}

template <typename T>
constexpr bool operator==(const T& lhs, const Luna::Type& rhs)
{
    if constexpr (std::is_same_v<T, Luna::Nil>)
    {
        if (rhs.isNil()) return true;
    }
    else if constexpr (std::is_same_v<T, Luna::Bool>)
    {
        if (rhs.isBool()) return rhs.asBool() == lhs;
    }
    else if constexpr (std::is_floating_point_v<T>)
    {
        if (rhs.isFloat()) return rhs.asFloat() == lhs;
    }
    else if constexpr (std::is_integral_v<T>)
    {
        if (rhs.isInteger()) return rhs.asInteger() == lhs;
    }
    else if constexpr (std::is_convertible_v<T, Luna::String>)
    {
        if (rhs.isString()) return rhs.asString() == lhs;
    }
    else if constexpr (std::is_convertible_v<T, Luna::Table>)
    {
        if (rhs.isTable()) return rhs.asTable() == lhs;
    }
    return false;
}

template <typename T>
constexpr bool operator==(const Luna::Type& lhs, const T& rhs)
{
    return rhs == lhs;
}

} // namespace Gng2D
