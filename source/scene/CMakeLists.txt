target_include_directories(Gng2D PUBLIC
    inc/)
target_sources(Gng2D PRIVATE
    src/assets.cpp
    src/scene.cpp
    src/input_handler.cpp
    src/entity_renderer.cpp)

# TESTS
target_sources(Gng2D_TESTS PRIVATE
    tst/input_handler.cpp)

