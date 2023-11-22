target_include_directories(Gng2D PUBLIC
    inc/)
target_sources(Gng2D PRIVATE
    src/scene.cpp
    src/input_handler.cpp
    src/entity_renderer.cpp
    src/relation_system.cpp)

if (DEFINED ENABLE_IMGUI)
    target_sources(Gng2D PRIVATE 
        src/debug/imgui_overlay.cpp)
endif()

# TESTS
target_sources(Gng2D_TESTS PRIVATE
    tst/input_handler.cpp
    tst/relation_system.cpp)

