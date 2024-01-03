target_include_directories(Gng2D PUBLIC
    inc/)
target_sources(Gng2D PRIVATE
    src/collider.cpp
    src/hoverable.cpp
    src/info.cpp
    src/lua_script.cpp
    src/relationship.cpp
    src/sprite.cpp
    src/transform.cpp)

#tests
target_sources(Gng2D_TESTS PRIVATE
    tst/collider.cpp
    tst/relationship.cpp
    tst/transform.cpp)
