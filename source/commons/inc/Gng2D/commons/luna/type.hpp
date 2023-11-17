#pragma once
#include "lua.hpp"
#include <string>
#include <variant>

namespace Gng2D::Luna {
enum TYPE {
    NIL,
    INT,
    FLOAT,
    STRING,
    BOOL,
    TABLE,
};
using Nil     = std::monostate;
using Integer = lua_Integer;
using Float   = lua_Number;
using String  = std::string;
using Bool    = bool;
struct Type;

struct TableRef
{
    ~TableRef();
    TableRef(const TableRef&);
    TableRef(TableRef&&);
    TableRef& operator=(const TableRef&);
    TableRef& operator=(TableRef&&);

    void set(const Type& key, const Type& value);
    Type get(const Type& key);

    friend bool operator==(const TableRef& lhs, const TableRef& rhs) { return lhs.ptr == rhs.ptr; }

  private:
    TableRef(lua_State*, int idx);
    friend struct Stack;
    friend struct State;

    lua_State*  L;
    int         regRef;
    const void* ptr;

    inline void markAsMovedFrom();
    inline bool ownsRef();
};

struct Type : std::variant<Nil, Integer, Float, String, Bool, TableRef>
{
    using std::variant<Nil, Integer, Float, String, Bool, TableRef>::variant;

    constexpr bool isNil() const { return std::holds_alternative<Nil>(*this); }
    constexpr bool isInteger() const { return std::holds_alternative<Integer>(*this); }
    constexpr bool isFloat() const { return std::holds_alternative<Float>(*this); }
    constexpr bool isString() const { return std::holds_alternative<String>(*this); }
    constexpr bool isBool() const { return std::holds_alternative<Bool>(*this); }
    constexpr bool isTable() const { return std::holds_alternative<TableRef>(*this); }
    auto&          asInteger() { return std::get<Integer>(*this); };
    auto&          asFloat() { return std::get<Float>(*this); };
    auto&          asString() { return std::get<String>(*this); };
    auto&          asBool() { return std::get<Bool>(*this); };
    auto&          asTable() { return std::get<TableRef>(*this); };
    const auto&    asInteger() const { return std::get<Integer>(*this); };
    const auto&    asFloat() const { return std::get<Float>(*this); };
    const auto&    asString() const { return std::get<String>(*this); };
    const auto&    asBool() const { return std::get<Bool>(*this); };
    const auto&    asTable() const { return std::get<TableRef>(*this); };
};

template <typename T>
constexpr bool operator==(const T& lhs, const Type& rhs)
{
    if constexpr (std::is_same_v<T, Nil>)
    {
        if (rhs.isNil()) return true;
    }
    else if constexpr (std::is_same_v<T, Bool>)
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
    else if constexpr (std::is_convertible_v<T, String>)
    {
        if (rhs.isString()) return rhs.asString() == lhs;
    }
    else if constexpr (std::is_same_v<T, TableRef>)
    {
        if (rhs.isTable()) return rhs.asTable() == lhs;
    }
    return false;
}

template <typename T>
constexpr bool operator==(const Type& lhs, const T& rhs)
{
    return rhs == lhs;
}
} // namespace Gng2D::Luna
