target_include_directories(Gng2D PUBLIC
    inc/)
target_sources(Gng2D PRIVATE
    src/app.cpp
    src/main_loop.cpp)

target_sources(Gng2D_EXEC PRIVATE 
    src/main.cpp)
