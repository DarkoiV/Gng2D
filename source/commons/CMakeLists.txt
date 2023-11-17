target_include_directories(Gng2D PUBLIC
    inc/)
target_sources(Gng2D PRIVATE 
    src/luna/stack.cpp
    src/luna/state.cpp
    src/luna/type.cpp
    src/args_vector.cpp
    src/repository.cpp)

# TESTS
target_sources(Gng2D_TESTS PRIVATE
    tst/luna/stack.cpp
    tst/luna/state.cpp
    tst/luna/type.cpp)
