target_include_directories(Gng2D PUBLIC
    inc/)
target_sources(Gng2D PRIVATE
    src/entity_recipe.cpp
    src/lua_api.cpp)

# TESTS
target_sources(Gng2D_TESTS PRIVATE
    tst/lua_api.cpp)

