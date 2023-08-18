target_include_directories(Gng2D PUBLIC 
    inc/)
target_sources(Gng2D PRIVATE
    src/coroutine.cpp
    src/font.cpp)

# Testing
target_sources(Gng2D_TESTS PRIVATE
    tst/coroutine_tests.cpp)

