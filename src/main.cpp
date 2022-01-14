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

// draw a single triangle that covers the full screen space and is clipped by the GPU
// doing this instead of two triangles to draw a screen quad avoids the overdraw of running the fragment shader in 2x2 pixel groups across the hypotenuse of the triangles
float vertices[] = {
	-1.0f, -3.0f, 0.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 1.0f,
	3.0f, 1.0f, 0.0f, 1.0f
};

// declare the texture to which the compute shader will redner into
GLuint tex_output;
GLuint shaderProgram, computeProgram;

sphere_s *sphereBuffer;
int sphereBufferLen;
light_s *lightBuffer;
int lightBufferLen;
vec4_t* primaryRayBuffer;
int primaryRayBufferLen;


GLuint ssboSpheres, ssboLights, ssboPrimaryRays;

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
	material.n = 500;
	material.ks = 0.6;
	rayTracer->getWorld()->getSpheres()->push_back(new Sphere(40, &origin, &color, &specularColor, &material));
	float* f = origin.get();
	float theta = 0;
	// create a vertial spiral of spheres in front of the camera
	for (int i = 0; i < 50; i++, theta += CONST_PI / 8.0)
	{
		f[0] = 150 * sin(theta);
		f[1] = i * 20;
		f[2] = 1000 + 150 * cos(theta);
		rayTracer->getWorld()->getSpheres()->push_back(new Sphere(20, &origin, &color, &specularColor, &material));
	}
	f[0] = 35;
	f[1] = -35;
	f[2] = 180;
	material.kd = 0.9;
	material.n = 50;
	material.ks = 0.2;
	color.get()[0] = 0;
	color.get()[1] = 0;
	color.get()[2] = 1;
	rayTracer->getWorld()->getSpheres()->push_back(new Sphere(30, &origin, &color, &specularColor, &material));

	sphereBufferLen = rayTracer->getWorld()->getSpheres()->size() * sizeof(sphere_s);
	//sphereBuffer = (sphere_s*)malloc(sphereBufferLen);

	// add lights to the scene
	color.set(0.8);
	origin.set(-100, 100, -100, 1);
	rayTracer->getWorld()->getLights()->push_back(new PointLight(&origin, &color));

	lightBufferLen = rayTracer->getWorld()->getLights()->size() * sizeof(light_s);
	//lightBuffer = (light_s*)malloc(lightBufferLen);

	// set up the depth buffer
	depthBuffer = new float[SCREEN_HEIGHT * SCREEN_WIDTH];

	// set up the primary ray buffer
	primaryRayBufferLen = sizeof(vec4_t) * SCREEN_HEIGHT * SCREEN_WIDTH;
	rayTracer->calculateEyeRays(SCREEN_WIDTH, SCREEN_HEIGHT, camera);
	//primaryRayBuffer = (vec4_t*)malloc(primaryRayBufferLen);
	
	//
	//pixels = new uint32_t[SCREEN_HEIGHT * SCREEN_WIDTH];

	return true;
}

// shader loading biolerplate taken from https://about-prog.com/opengl/ogl-triangle
int init_opengl()
{
	

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

	// generate the texture into which the compute shader will render
	glGenTextures(1, &tex_output);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex_output);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glBindImageTexture(0, tex_output, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	std::ifstream ivs("vs.glsl");
	std::string vs((std::istreambuf_iterator<char>(ivs)), (std::istreambuf_iterator<char>()));
	const char* vsc = vs.c_str();
	
	std::ifstream ifs("fs.glsl");
	std::string fs((std::istreambuf_iterator<char>(ifs)), (std::istreambuf_iterator<char>()));
	const char* fsc = fs.c_str();
	
	std::ifstream ics("raytrace.glsl");
	std::string cs((std::istreambuf_iterator<char>(ics)), (std::istreambuf_iterator<char>()));
	const char* csc = cs.c_str();
	
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsc, NULL);
	glCompileShader(vertexShader);
	
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsc, NULL);
	glCompileShader(fragmentShader);

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	//glUseProgram(shaderProgram);

	GLint isLinked = 0;
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, &infoLog[0]);

		// The program is useless now. So delete it.
		glDeleteProgram(shaderProgram);

		// Provide the infolog in whatever manner you deem best.
		SDL_Log("ERROR linking fragment and vertex shader program: %s\n", infoLog.data());
		// Exit with failure.
		return 1;
	}

	GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(computeShader, 1, &csc, NULL);
	glCompileShader(computeShader);

	computeProgram = glCreateProgram();
	glAttachShader(computeProgram, computeShader);
	glLinkProgram(computeProgram);

	isLinked = 0;
	glGetProgramiv(computeProgram, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE)
	{
		GLint maxLength = 0;
		glGetProgramiv(computeProgram, GL_INFO_LOG_LENGTH, &maxLength);

		// The maxLength includes the NULL character
		std::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(computeProgram, maxLength, &maxLength, &infoLog[0]);

		// The program is useless now. So delete it.
		glDeleteProgram(computeProgram);

		// Provide the infolog in whatever manner you deem best.
		SDL_Log("ERROR linking compute program: %s\n", infoLog.data());
		// Exit with failure.
		return 1;
	}

	// create the shader storage buffer object to send sphere data to the compute shader
	glGenBuffers(1, &ssboSpheres);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboSpheres);
	//glBufferData(GL_SHADER_STORAGE_BUFFER, sphereBufferLen, sphereBuffer, GL_DYNAMIC_DRAW);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sphereBufferLen, sphereBuffer, GL_STREAM_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboSpheres);
	//sphereBuffer = (sphere_s*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sphereBufferLen, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	sphereBuffer = (sphere_s*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sphereBufferLen, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	rayTracer->getWorld()->genSphereBuffer(sphereBuffer, rayTracer->getWorld()->getSpheres()->size() * sizeof(sphere_s));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// create the shader storage buffer object to send light data to the compute shader
	glGenBuffers(1, &ssboLights);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboLights);
	glBufferData(GL_SHADER_STORAGE_BUFFER, lightBufferLen, lightBuffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssboLights);
	lightBuffer = (light_s *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, lightBufferLen, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	rayTracer->getWorld()->genLightBuffer(lightBuffer, rayTracer->getWorld()->getLights()->size() * sizeof(light_s));
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

	// create the shader storage buffer object to send primary ray data to the compute shader
	glGenBuffers(1, &ssboPrimaryRays);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboPrimaryRays);
	glBufferData(GL_SHADER_STORAGE_BUFFER, primaryRayBufferLen, primaryRayBuffer, GL_DYNAMIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssboPrimaryRays);
	primaryRayBuffer = (vec4_t *)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, primaryRayBufferLen, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	rayTracer->genEyeRaysBuffer(primaryRayBuffer, primaryRayBufferLen, SCREEN_WIDTH, SCREEN_HEIGHT);
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

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

#if 0		
		if (rayTracer->rayTraceSceneToPixelBuffer(&pixels, &depthBuffer, SCREEN_WIDTH, SCREEN_HEIGHT, camera)) {
			return;
		}
#endif
		
		// update the sphere buffer
		// wait for all compute units to finish operating on the buffer, then retrieve a pointer to it and update it
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboSpheres);
		sphereBuffer = (sphere_s*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sphereBufferLen, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		rayTracer->getWorld()->genSphereBuffer(sphereBuffer, rayTracer->getWorld()->getSpheres()->size() * sizeof(sphere_s));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		// update the light buffer
		glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboLights);
		lightBuffer = (light_s*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, lightBufferLen, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
		rayTracer->getWorld()->genLightBuffer(lightBuffer, rayTracer->getWorld()->getLights()->size() * sizeof(light_s));
		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

		//rayTracer->genEyeRaysBuffer(primaryRayBuffer, primaryRayBufferLen, SCREEN_WIDTH, SCREEN_HEIGHT);
	}

	
	SDL_UnlockTexture(buffer);
#if 0
	SDL_RenderCopy(renderer, buffer, NULL, NULL);
	SDL_RenderPresent(renderer);
#endif
	
	// prepare to use shaders for ray tracing
	glUseProgram(computeProgram);
	glDispatchCompute((GLuint)SCREEN_WIDTH, (GLuint)SCREEN_HEIGHT, 1);

	// wait to finish processing image before proceeding with read on the texture
	//glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	glClear(GL_COLOR_BUFFER_BIT);
	
	// render the triangle with the compute shader output as a texture
	glUseProgram(shaderProgram);
	glActiveTexture(GL_TEXTURE0);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glBindTexture(GL_TEXTURE_2D, tex_output);
	glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/sizeof(float));

	// double buffer present
	SDL_GL_SwapWindow(window);
	
}


void free_resources() {
	/* FILLED IN LATER */
	SDL_DestroyTexture(buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);

	delete camera;
	delete rayTracer;

	free(sphereBuffer);
	free(lightBuffer);
	free(primaryRayBuffer);
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
	if (!init_resources(window) || init_opengl())
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