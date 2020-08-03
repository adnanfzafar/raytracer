// main.cpp : Defines the entry point for the application.
//

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

#include <intrin.h> // check CPU for ISA features via __cpuid(...)

#include <fstream>

#include "CPUId.h"
#include "Vector4f.h"
#include "Primitive.h"
#include "Sphere.h"
#include "Light.h"
#include "PointLight.h"
#include "World.h"
#include "RayTracer.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define BYTES_PER_PIXEL 3
#define BYTES_PER_ROW SCREEN_WIDTH*BYTES_PER_PIXEL


SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* buffer;

uint32_t* pixels;
int pitch;

float* depthBuffer;

Camera* camera;
RayTracer* rayTracer;
CPUId cpuid;
/*
static vec3_t eye = { 0,0,1 };
static vec3_t up = { 0,1,0 };
static vec3_t eye_origin = { 0,0,-150 };
*/
static float fov = 90;

bool init_resources(SDL_Window* window) {
	/* FILLED IN LATER */

	renderer = NULL;
	buffer = NULL;
	pixels = NULL;
	camera = NULL;
	rayTracer = NULL;

	if (!window)
		return false;

	// check CPU for SSE and SSE3
	if (!cpuid.SSE || !cpuid.SSE3)
	{
		std::cout << "ERROR: CPU does not support required features SSE and SSE3!" << std::endl;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer)
		buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	if (!buffer)
		return false;

	// setup the raytracer (eye, up, eye_origin, fov)
	camera = new Camera(Vector4f(0, 0, 1, 0), Vector4f(0, 1, 0, 0), Vector4f(0, 0, 0, 1), fov);
	//rayTracer = new RayTracer(Vector4f(0,0,1,0), Vector4f(0,1,0,0), Vector4f(0,0,-150,1), fov); // for sphere spiral
	rayTracer = new RayTracer(); // for two spheres
	if (!rayTracer)
		return false;

	// setup the scene
	// sphere spiral
	/*
	Vector4f sphere_origin(0, 0, 0, 1);
	float* f = sphere_origin.get();
	float theta = 0;
	for (int i = 0; i < 16; i++, theta += CONST_PI/8.0)
	{
		f[0] = i*5;
		f[1] = 50 * sin(theta);
		f[2] = 50 * cos(theta);
		rayTracer->getWorld()->getPrimitives()->push_back(new Sphere(10, &sphere_origin));
	}
	*/

	// TODO: properly declare each instance of the sphere to match the Sphere constructors, including colors and material properties
	Vector4f origin(0, 0, 200, 1);
	material_s material;
	Vector4f color(1.0, 0, 0, 1.0);
	Vector4f specularColor(1.0, 1.0, 1.0, 1.0);
	material.ka = 0.15;
	material.kd = 0.7;
	material.n = 200;
	material.ks = 0.6;
	rayTracer->getWorld()->getPrimitives()->push_back(new Sphere(40, &origin, &color, &specularColor, &material));
	float* f = origin.get();
	f[0] = 35;
	f[1] = -35;
	f[2] = 180;
	material.kd = 0.9;
	material.n = 50;
	material.ks = 0.2;
	color.get()[0] = 0;
	color.get()[1] = 0;
	color.get()[2] = 1;
	rayTracer->getWorld()->getPrimitives()->push_back(new Sphere(30, &origin, &color, &specularColor, &material));

	// add lights to the scene
	color.set(0.8);
	origin.set(-100, 100, -100, 1);
	rayTracer->getWorld()->getLights()->push_back(new PointLight(&origin, &color));

	// set up the depth buffer
	depthBuffer = new float[SCREEN_HEIGHT * SCREEN_WIDTH];
	
	//
	//pixels = new uint32_t[SCREEN_HEIGHT * SCREEN_WIDTH];

	return true;
}

// shader loading biolerplate taken from https://about-prog.com/opengl/ogl-triangle
int init_opengl()
{
	float vertices[] = {
		-0.5f, -0.5f, 0.0f, 1.0f,
		 0.5f, -0.5f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f, 1.0f
	};

	/* Extension wrangler initialising */
	
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
		return EXIT_FAILURE;
	}

	glClearColor(1.0,1.0,1.0,1.0);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	std::ifstream ivs("vs.glsl");
	std::string vs((std::istreambuf_iterator<char>(ivs)), (std::istreambuf_iterator<char>()));
	const char* vsc = vs.c_str();
	std::ifstream ifs("fs.glsl");
	std::string fs((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	const char* fsc = fs.c_str();

	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsc, NULL);
	glCompileShader(vertexShader);

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsc, NULL);
	glCompileShader(fragmentShader);

	GLuint shaderProgram = glCreateProgram();

	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glUseProgram(shaderProgram);

	return 0;
}

void render(SDL_Window* window) {
	static long ticks = 0;
	static Uint64 time_prev, time_now;
	static int frame_count = 0;
	/* FILLED IN LATER */
	
	if (0 != SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch))
	{
		cout << "ERROR: Failed to lock texture!" << endl;
		SDL_UnlockTexture(buffer);
		return;
	}

	if (camera && rayTracer) {
		(++frame_count) %= 100;
		if (frame_count == 0)
		{
			time_prev = time_now;
			time_now = SDL_GetPerformanceCounter();
			SDL_Log("Avg frame time = %f ms (%f fps)", (double)((time_now - time_prev)*1000) / (100*SDL_GetPerformanceFrequency()), (double)(100 * SDL_GetPerformanceFrequency())/ ((time_now - time_prev)));
		}

		rayTracer->getWorld()->update(ticks);

		if (rayTracer->rayTraceSceneToPixelBuffer(&pixels, &depthBuffer, SCREEN_WIDTH, SCREEN_HEIGHT, camera)) {
			return;
		}
	}

	
	SDL_UnlockTexture(buffer);
	
	SDL_RenderCopy(renderer, buffer, NULL, NULL);
	SDL_RenderPresent(renderer);
	
	/*
	// prepare to use shaders for ray tracing
	glClear(GL_COLOR_BUFFER_BIT);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	SDL_GL_SwapWindow(window);
	*/
}

void free_resources() {
	/* FILLED IN LATER */
	SDL_DestroyTexture(buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	delete camera;
	delete rayTracer;
}

void mainLoop(SDL_Window* window) {
	while (true) {
		SDL_Event ev;
		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT)
				return;
		}
		render(window);
	}
}

int main(int argc, char* argv[]) {
	SDL_SetMainReady();
	/* SDL-related initialising functions */
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow("Ray Tracer",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_GL_CreateContext(window);

	// prepare to use shaders for ray tracing
	//init_opengl();

	/* When all init functions run without errors,
	   the program can initialise the resources */
	if (!init_resources(window))
		return EXIT_FAILURE;

	/* We can display something if everything goes OK */
	mainLoop(window);

	/* If the program exits in the usual way,
	   free resources and exit with a success */
	free_resources();
	return EXIT_SUCCESS;
}

// dont need WinMain after having disabled SDLs redefinition of main per https://wiki.libsdl.org/SDL_SetMainReady
/*
int WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nShowCmd
) {
	return main(0, NULL);
}
*/