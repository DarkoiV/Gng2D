target_include_directories(Gng2D PUBLIC 
    inc/)

# Testing
target_sources(Gng2D_TESTS PRIVATE
    tst/overlap_system_tests.cpp)

