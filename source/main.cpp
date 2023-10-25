#include <iostream>
#include "lua.hpp"

int main()
{
    auto* L = luaL_newstate();
    luaL_dostring(L, "x = 10");
    lua_getglobal(L, "x");
    double x = lua_tonumber(L, -1);
    std::cout << "Hello Gng2D: " << x << std::endl;
    return 0;
}
