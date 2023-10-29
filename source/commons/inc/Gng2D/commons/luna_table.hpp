#pragma once
#include "lua.hpp"
#include <string>
#include <optional>

namespace Gng2D
{
struct LunaTable 
{
    ~LunaTable();

private:
    lua_State* L;

    LunaTable(lua_State*);
    [[nodiscard]] std::optional<LunaTable> 
        load(lua_State*, const std::string&);
};
}

