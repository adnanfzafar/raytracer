# CMakeList.txt : CMake project for raytracer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# add openmp compiler directives and library locations
set(OpenMP_LIBRARY_DIR "d:/Program Files (x86)/Microsoft Visual Studio/2019/Community/VC/Tools/Llvm/x64/lib")
set(OpenMP_CXX_FLAGS "-Xclang -fopenmp -v")

# add SSE* directives
set(SSEx_CXX_FLAGS "-msse3")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS} ${SSEx_CXX_FLAGS}")

# Tell CMake where to find the OpenMP libraries.
link_directories(${OpenMP_LIBRARY_DIR})

# Add source to this project's executable.
add_executable (raytracer  "raytracer.h" "math.h" "main.cpp" "main.h"  "world.h"  "primitives.h"   "hardware.h"  "camera.h")

# TODO: Add tests and install targets if needed.

# add in GLEW and SDL headers
target_include_directories(raytracer PUBLIC "D:/dev/libraries/glew-2.1.0-win32/glew-2.1.0/include")
target_include_directories(raytracer PUBLIC "D:/dev/libraries/SDL2-devel-2.0.10-VC/SDL2-2.0.10/include")

# add in GLEW and SDL static libraries
target_link_libraries(raytracer PUBLIC "D:/dev/libraries/glew-2.1.0-win32/glew-2.1.0/lib/Release/x64/glew32.lib")
target_link_libraries(raytracer PUBLIC "D:/dev/libraries/SDL2-devel-2.0.10-VC/SDL2-2.0.10/lib/x64/sdl2.lib")

# add in openmp libaraies
target_link_libraries(raytracer PUBLIC "libomp.lib")