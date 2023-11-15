target_include_directories(Gng2D PUBLIC
    inc/)
target_sources(Gng2D PRIVATE 
    src/args_vector.cpp
    src/luna_stack.cpp
    src/luna_state.cpp
    src/luna_type.cpp
    src/repository.cpp)

# TESTS
target_sources(Gng2D_TESTS PRIVATE
    tst/luna_state.cpp)
