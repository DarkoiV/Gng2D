target_include_directories(Gng2D PUBLIC
    inc/)
target_sources(Gng2D PRIVATE
    src/app.cpp
    src/luna.cpp)

# TESTS
target_sources(Gng2D_TESTS PRIVATE
    tst/luna.cpp)

