#pragma once

/* Using standard C++ output libraries */
#include <cstdlib>
#include <iostream>
using namespace std;


#include <windows.h> 
/* Use glew.h instead of gl.h to get all the GL prototypes declared */
#include <GL/glew.h>
/* Using SDL2 for the base window and OpenGL context init */
#define SDL_MAIN_HANDLED // this disabled SDLs need to redefine main
#include <SDL2/SDL.h>

#include "math.h"
#include "primitives.h"
#include "world.h"
#include "raytracer.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define BYTES_PER_PIXEL 3
#define BYTES_PER_ROW SCREEN_WIDTH*BYTES_PER_PIXEL