target_include_directories(Gng2D PUBLIC 
    inc/)
target_sources(Gng2D PRIVATE
    src/entity_renderer.cpp
    src/mouse_system.cpp
    src/position_system.cpp
    src/relation_system.cpp
    src/scene.cpp)

