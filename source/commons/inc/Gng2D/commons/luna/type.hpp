#pragma once
#include "Gng2D/commons/assert.hpp"
#include "lua.hpp"
#include <memory>
#include <string>
#include <variant>
#include <vector>

namespace Gng2D::Luna {
enum TYPE {
    NIL,
    INT,
    FLOAT,
    STRING,
    BOOL,
    TABLE,
    FUNCTION,
    USERDATA,
};
using Nil     = std::monostate;
using Integer = lua_Integer;
using Float   = lua_Number;
using String  = std::string;
using Bool    = bool;
struct Type;

struct Ref
{
    Ref(lua_State*, int idx);
    ~Ref();
    Ref(const Ref&)            = delete;
    Ref(Ref&&)                 = delete;
    Ref& operator=(const Ref&) = delete;
    Ref& operator=(Ref&&)      = delete;

    int get() const { return ref; };

  private:
    lua_State* L;
    int        ref;
};
using SharedRef = std::shared_ptr<Ref>;

struct TableRef
{
    ~TableRef() = default;
    TableRef(const TableRef&);
    TableRef(TableRef&&);
    TableRef& operator=(const TableRef&);
    TableRef& operator=(TableRef&&);

    void createSubTable(const Type& key);
    void set(const Type& key, const Type& value);
    Type get(const Type& key) const;

    friend bool operator==(const TableRef& lhs, const TableRef& rhs) { return lhs.ptr == rhs.ptr; }

    struct Iterator
    {
        using iterator_category = std::input_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = std::pair<Type, Type>;

        ~Iterator() = default;
        Iterator(const TableRef&);
        Iterator(const Iterator&);
        Iterator(Iterator&&);

        Iterator& operator=(const Iterator&) = delete;
        Iterator& operator=(Iterator&&)      = delete;

        Iterator& operator++();
        Iterator  operator++(int);

        value_type operator*();

        friend bool operator==(const Iterator& lhs, const Iterator& rhs)
        {
            GNG2D_ASSERT(lhs.tableRef == rhs.tableRef);
            return (lhs.isThereNextElement == rhs.isThereNextElement) and
                   (lhs.isThereNextElement == false or areKeysSame(lhs, rhs));
        }

        friend bool operator!=(const Iterator& lhs, const Iterator& rhs)
        {
            GNG2D_ASSERT(lhs.tableRef == rhs.tableRef);
            return (lhs.isThereNextElement != rhs.isThereNextElement) and
                   (lhs.isThereNextElement != false or not areKeysSame(lhs, rhs));
        }

      private:
        lua_State*      thread{};
        const TableRef& tableRef;
        SharedRef       threadRef;
        bool            isThereNextElement;

        friend TableRef;
        Iterator(lua_State* L, const TableRef&);

        static bool areKeysSame(const Iterator&, const Iterator&);
    };

    Iterator begin() const { return Iterator(L, *this); };
    Iterator end() const { return Iterator(*this); };

  private:
    TableRef(lua_State*, int idx);
    friend struct Stack;
    friend struct State;

    lua_State*  L;
    SharedRef   regRef;
    const void* ptr;
};

struct FunctionRef
{
    ~FunctionRef() = default;
    FunctionRef(const FunctionRef&);
    FunctionRef(FunctionRef&&);
    FunctionRef& operator=(const FunctionRef&);
    FunctionRef& operator=(FunctionRef&&);

    friend bool operator==(const FunctionRef& lhs, const FunctionRef& rhs)
    {
        return lhs.ptr == rhs.ptr;
    };

  private:
    FunctionRef(lua_State*, int idx);
    friend struct Stack;
    friend struct State;

    lua_State*  L;
    SharedRef   regRef;
    const void* ptr;
};

struct UserdataRef
{
    ~UserdataRef() = default;
    UserdataRef(const UserdataRef&);
    UserdataRef(UserdataRef&&);
    UserdataRef& operator=(const UserdataRef&);
    UserdataRef& operator=(UserdataRef&&);

    void* get() { return value; }

    friend bool operator==(const UserdataRef& lhs, const UserdataRef& rhs)
    {
        return lhs.value == rhs.value;
    };

  private:
    UserdataRef(lua_State*, int idx);
    friend struct Stack;
    friend struct State;

    lua_State* L;
    SharedRef  regRef;
    void*      value;
};

using TypesVariant =
    std::variant<Nil, Integer, Float, String, Bool, TableRef, FunctionRef, UserdataRef>;
struct Type : TypesVariant
{
    using TypesVariant::variant;

    constexpr bool isNil() const { return std::holds_alternative<Nil>(*this); }
    constexpr bool isInteger() const { return std::holds_alternative<Integer>(*this); }
    constexpr bool isFloat() const { return std::holds_alternative<Float>(*this); }
    constexpr bool isString() const { return std::holds_alternative<String>(*this); }
    constexpr bool isBool() const { return std::holds_alternative<Bool>(*this); }
    constexpr bool isTable() const { return std::holds_alternative<TableRef>(*this); }
    constexpr bool isFunction() const { return std::holds_alternative<FunctionRef>(*this); }
    constexpr bool isUserdata() const { return std::holds_alternative<UserdataRef>(*this); }
    auto&          asInteger() { return std::get<Integer>(*this); };
    auto&          asFloat() { return std::get<Float>(*this); };
    auto&          asString() { return std::get<String>(*this); };
    auto&          asBool() { return std::get<Bool>(*this); };
    auto&          asTable() { return std::get<TableRef>(*this); };
    auto&          asFunction() { return std::get<FunctionRef>(*this); };
    auto&          asUserdata() { return std::get<UserdataRef>(*this); };
    const auto&    asInteger() const { return std::get<Integer>(*this); };
    const auto&    asFloat() const { return std::get<Float>(*this); };
    const auto&    asString() const { return std::get<String>(*this); };
    const auto&    asBool() const { return std::get<Bool>(*this); };
    const auto&    asTable() const { return std::get<TableRef>(*this); };
    const auto&    asFunction() const { return std::get<FunctionRef>(*this); };
    const auto&    asUserdata() const { return std::get<UserdataRef>(*this); };

    template <typename T>
    bool tryAssignTo(T& target);
};

using TypeVector = std::vector<Type>;

} // namespace Gng2D::Luna
#include "type.ipp"
