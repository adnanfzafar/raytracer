// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* buffer;

uint32_t* pixels;
int pitch;

float* depthBuffer;

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
	//rayTracer = new RayTracer(Vector4f(0,0,1,0), Vector4f(0,1,0,0), Vector4f(0,0,-150,1), fov); // for sphere spiral
	rayTracer = new RayTracer(Vector4f(0,0,1,0), Vector4f(0,1,0,0), Vector4f(0,0,0,1), fov); // for two spheres
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

	Vector4f sphere_origin(0, 0, 200, 1);
	rayTracer->getWorld()->getPrimitives()->push_back(new Sphere(40, &sphere_origin));
	float* f = sphere_origin.get();
	f[0] = 35;
	f[1] = -35;
	f[2] = 180;
	rayTracer->getWorld()->getPrimitives()->push_back(new Sphere(30, &sphere_origin));

	// set up the depth buffer
	depthBuffer = new float[SCREEN_HEIGHT * SCREEN_WIDTH];

	return true;
}



void render(SDL_Window* window) {
	static int pixel_index;
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

	pixel_index = 0;
	// RENDER TO PIXELS
	
	/*
	for (y = 0; y < SCREEN_HEIGHT; y++)
	{
		pixel_index = SCREEN_HEIGHT * y;
		for (x = 0; x < SCREEN_WIDTH; x++)
		{
			//pixel_index = SCREEN_HEIGHT * y + x;

			//pixels[pixel_index] = 127 << 8;
			//pixels[pixel_index] |= 127 << 16;
			//pixels[pixel_index] |= 127 << 24;
			pixels[pixel_index + x] = 2139062016;
		}
	}
	*/

	if (rayTracer) {
		(++frame_count) %= 100;
		if (frame_count == 0)
		{
			time_prev = time_now;
			time_now = SDL_GetPerformanceCounter();
			SDL_Log("Avg frame time = %f ms (%f fps)", (double)((time_now - time_prev)*1000) / (100*SDL_GetPerformanceFrequency()), (double)(100 * SDL_GetPerformanceFrequency())/ ((time_now - time_prev)));
		}

		rayTracer->getWorld()->update(ticks);

		if (rayTracer->rayTraceDepthSceneToPixelBuffer(&pixels, &depthBuffer, SCREEN_WIDTH, SCREEN_HEIGHT)) {
			return;
		}
	}

	SDL_UnlockTexture(buffer);

	SDL_RenderCopy(renderer, buffer, NULL, NULL);
	SDL_RenderPresent(renderer);
}

void free_resources() {
	/* FILLED IN LATER */
	SDL_DestroyTexture(buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
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
	window = SDL_CreateWindow("My First Ray Tracer",
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, SCREEN_HEIGHT,
		SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
	SDL_GL_CreateContext(window);

	/* Extension wrangler initialising */
	/*
	GLenum glew_status = glewInit();
	if (glew_status != GLEW_OK) {
		cerr << "Error: glewInit: " << glewGetErrorString(glew_status) << endl;
		return EXIT_FAILURE;
	}
	*/

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