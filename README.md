# Gng2D
Simple 2D game engine written in C++
![obraz](https://github.com/DarkoiV/Gng2D/assets/25897608/b4a01b7f-8f83-4961-9afe-0824728b72c1)


## Build Requirements
- `C++20 Compiler`  
- `Cmake 3.22+`  
- `SDL2 dev libraries`  

### Linux
To build on linux simply clone repository, create build folder, and run CMake && make inside.  
For example:
```bash
git clone https://github.com/DarkoiV/Gng2D;
cd Gng2D;
mkdir build && cd build;
cmake .. && make -j$(nproc);
```
