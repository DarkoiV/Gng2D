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

    void                        doFile(const std::string& path, const std::string& env = "");
    std::optional<int>          readGlobalInt(const std::string&);
    std::optional<std::string>  readGlobalString(const std::string&);

    template<typename T>
    void setVar(const std::string&, T& var);

private:
    lua_State* L = luaL_newstate();

    void setEnv(const std::string& env);
};

template<typename T>
void Luna::setVar(const std::string& name, T& var)
{
    if constexpr (std::is_integral_v<T>)
    {
        if (auto value = readGlobalInt(name); value)
        {
            var = *value;
            LOG::INFO(name, "set to", var);
        }
    }
    if constexpr (std::is_same_v<T, std::string>)
    {
        if (auto value = readGlobalString(name); value)
        {
            var = std::move(*value);
            LOG::INFO(name, "set to", var);
        }
    }
}

}

