#pragma once
#include "Gng2D/core/log.hpp"
#include "lua.hpp"
#include <string>
#include <optional>

namespace Gng2D
{
struct Luna 
{
    Luna();
    ~Luna();

    void                        doFile(const std::string& path, const std::string& env = "");
    void                        doString(const std::string& str, const std::string& env = "");
    std::optional<int>          readInt(const std::string&);
    std::optional<double>       readFloat(const std::string&);
    std::optional<std::string>  readString(const std::string&);
    std::optional<bool>         readBool(const std::string&);
    void                        createInt(const std::string&, int);
    void                        createFloat(const std::string&, double);
    void                        createString(const std::string& name, const std::string& var);
    void                        createBool(const std::string&, bool);

    template<typename T>
    bool readToVar(const std::string&, T& var);

private:
    lua_State* L = luaL_newstate();

    void setEnv(const std::string& env);
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
    else
    {
        []<bool flag = false>()
        {
            static_assert(flag, "Type not supported");
        }();
    }
    LOG::DEBUG(name, "=", var);
    return true;
}
}

