#include "Gng2D/commons/luna/stack.hpp"
#include "Gng2D/commons/assert.hpp"
#include "Gng2D/commons/log.hpp"

using namespace Gng2D::Luna;

namespace {
template <typename>
struct tag
{
};

template <typename T, typename V>
struct get_index;

template <typename T, typename... Ts>
struct get_index<T, std::variant<Ts...>>
    : std::integral_constant<std::size_t, std::variant<tag<Ts>...>(tag<T>()).index()>
{
};
} // namespace

void Stack::pushNil()
{
    lua_pushnil(L);
}

void Stack::pushInt(Integer value)
{
    lua_pushinteger(L, value);
}

void Stack::pushFloat(Float value)
{
    lua_pushnumber(L, value);
}

void Stack::pushString(const String& value)
{
    lua_pushstring(L, value.c_str());
}

void Stack::pushBool(Bool value)
{
    lua_pushboolean(L, value);
}

void Stack::pushTable(const TableRef& tr)
{
    const auto RES = lua_rawgeti(L, LUA_REGISTRYINDEX, tr.regRef->get());
    GNG2D_ASSERT(RES == LUA_TTABLE, "No table at stored ref in TableRef");
}

void Stack::pushFunction(const FunctionRef& fr)
{
    const auto RES = lua_rawgeti(L, LUA_REGISTRYINDEX, fr.regRef->get());
    GNG2D_ASSERT(RES == LUA_TFUNCTION, "No table at stored ref in FunctionRef");
}

void Stack::pushUserdata(const UserdataRef& ur)
{
    const auto RES = lua_rawgeti(L, LUA_REGISTRYINDEX, ur.regRef->get());
    GNG2D_ASSERT(RES == LUA_TUSERDATA, "No table at stored ref in UserdataRef");
}

void Stack::pushGlobal(const String& name)
{
    lua_getglobal(L, name.c_str());
}

void Stack::setMetaTable(int indx, const TableRef& table)
{
    push(table);
    indx = indx > 0 ? indx : indx - 1;
    lua_setmetatable(L, indx);
}

void Stack::pushMetaTable(int indx)
{
    auto RES = lua_getmetatable(L, indx);
    GNG2D_ASSERT(RES, "No metatabable found");
    if (not RES) pushNil();
}

void Stack::push(const Type& value)
{
    switch (value.index())
    {
    case TYPE::NIL:
        lua_pushnil(L);
        return;
    case TYPE::INT:
        pushInt(value.asInteger());
        return;
    case TYPE::FLOAT:
        pushFloat(value.asFloat());
        return;
    case TYPE::STRING:
        pushString(value.asString());
        return;
    case TYPE::BOOL:
        pushBool(value.asBool());
        return;
    case TYPE::TABLE:
        pushTable(value.asTable());
        return;
    case TYPE::FUNCTION:
        pushFunction(value.asFunction());
        return;
    case TYPE::USERDATA:
        pushUserdata(value.asUserdata());
        return;
    }
    LOG::ERROR("Fallthrough on push");
}

void Stack::setGlobal(const std::string& name)
{
    lua_setglobal(L, name.c_str());
}

Type Stack::read(int n) const
{
    auto type = lua_type(L, n);
    switch (type)
    {
    case LUA_TNIL:
        return Nil{};
    case LUA_TNUMBER:
        if (lua_isinteger(L, n)) return Integer{lua_tointeger(L, n)};
        else return Float{lua_tonumber(L, n)};
    case LUA_TSTRING:
        return String{lua_tostring(L, n)};
    case LUA_TBOOLEAN:
        return bool{static_cast<bool>(lua_toboolean(L, n))};
    case LUA_TTABLE:
        return TableRef(L, n);
    case LUA_TFUNCTION:
        return FunctionRef(L, n);
    case LUA_TUSERDATA:
        return UserdataRef(L, n);
    }

    LOG::DEBUG("Is of non readable type:", type);
    return Nil{};
}

TYPE Stack::is(int n) const
{
    auto type = lua_type(L, n);
    switch (type)
    {
    case LUA_TNIL:
        return TYPE::NIL;
    case LUA_TNUMBER:
        if (lua_isinteger(L, n)) return TYPE::INT;
        else return TYPE::FLOAT;
    case LUA_TSTRING:
        return TYPE::STRING;
    case LUA_TBOOLEAN:
        return TYPE::BOOL;
    case LUA_TTABLE:
        return TYPE::TABLE;
    case LUA_TFUNCTION:
        return TYPE::FUNCTION;
    case LUA_TUSERDATA:
        return TYPE::USERDATA;
    };

    LOG::DEBUG("Var is of non readable type");
    return TYPE::NIL;
}

void Stack::pop(int n)
{
    lua_pop(L, n);
}

int Stack::top() const
{
    return lua_gettop(L);
}

void Stack::newTable()
{
    lua_newtable(L);
}

void Stack::setTableField(const Type& key, const Type& value, int tableIndx)
{
    GNG2D_ASSERT(lua_istable(L, tableIndx), "setTableField requies table at index", tableIndx);
    tableIndx = tableIndx < 0 ? tableIndx - 2 : tableIndx;
    push(key);
    push(value);
    lua_rawset(L, tableIndx);
}

void Stack::setTableFieldFS(int tableIndx)
{
    GNG2D_ASSERT(lua_istable(L, tableIndx), "setTableField requies table at index", tableIndx);
    lua_rawset(L, tableIndx);
}

void Stack::pushTableField(const Type& key, int tableIndx)
{
    GNG2D_ASSERT(lua_istable(L, tableIndx), "pushTableField requies table at index", tableIndx);
    tableIndx = tableIndx < 0 ? tableIndx - 1 : tableIndx;
    push(key);
    lua_rawget(L, tableIndx);
}

void Stack::pushTableFieldFS(int tableIndx)
{
    GNG2D_ASSERT(lua_istable(L, tableIndx), "pushTableField requies table at index", tableIndx);
    lua_rawget(L, tableIndx);
}

int Stack::callFunction(const FunctionRef& fn, std::vector<Type> args)
{
    push(fn);
    return callFunctionFS(std::move(args));
}

int Stack::callFunctionFS(std::vector<Type> args)
{
    int retNo = top();
    for (auto& arg: args)
        push(arg);
    const auto RES = lua_pcall(L, args.size(), LUA_MULTRET, 0);
    GNG2D_ASSERT(RES == LUA_OK, lua_tostring(L, -1));
    retNo = top() - retNo + 1; // +1 'cause pcall pops function too

    return retNo;
}

StackLock::StackLock(lua_State* state)
    : L(state)
{
    top_ = lua_gettop(L);
}

StackLock::~StackLock()
{
    GNG2D_ASSERT(top_ <= lua_gettop(L), "Stack lock went out of scope!");
    lua_settop(L, top_);
}
