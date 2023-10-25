#pragma once
#include "lua.hpp"
#include <string>
#include <optional>

namespace Gng2D
{
struct Luna 
{
    void                        doFile(const std::string&);
    std::optional<int>          readGlobalInt(const std::string&);
    std::optional<std::string>  readGlobalString(const std::string&);

private:
    lua_State* L = luaL_newstate();
};
}

