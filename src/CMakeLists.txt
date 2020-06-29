# CMakeList.txt : CMake project for raytracer, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (raytracer "raytracer.cpp" "raytracer.h" "math.h" "main.cpp" "main.h" "world.cpp" "world.h" "primitives.cpp" "primitives.h" "math.cpp" )

# TODO: Add tests and install targets if needed.

# add in GLEW and SDL headers
target_include_directories(raytracer PUBLIC "D:/dev/libraries/glew-2.1.0-win32/glew-2.1.0/include")
target_include_directories(raytracer PUBLIC "D:/dev/libraries/SDL2-devel-2.0.10-VC/SDL2-2.0.10/include")

# add in GLEW and SDL static libraries
target_link_libraries(raytracer PUBLIC "D:/dev/libraries/glew-2.1.0-win32/glew-2.1.0/lib/Release/x64/glew32.lib")
target_link_libraries(raytracer PUBLIC "D:/dev/libraries/SDL2-devel-2.0.10-VC/SDL2-2.0.10/lib/x64/sdl2.lib")
#D:\dev\libraries\glew-2.1.0-win32\glew-2.1.0\lib\Release\Win32;D:\dev\libraries\SDL2-devel-2.0.10-VC\SDL2-2.0.10\lib\x86;