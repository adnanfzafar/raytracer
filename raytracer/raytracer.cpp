// raytracer.cpp : Defines the entry point for the application.
//

#include "raytracer.h"

//#undef main 

using namespace std;

//#include "main.h"

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Texture* buffer;

uint32_t* pixels;
int pitch;

bool init_resources(SDL_Window* window) {
	/* FILLED IN LATER */
	renderer = NULL;
	buffer = NULL;
	pixels = NULL;

	if (!window)
		return false;

	renderer = SDL_CreateRenderer(window, -1, 0);
	if (renderer)
		buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_BGRA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);

	//pixels = new int(SCREEN_WIDTH * SCREEN_HEIGHT * 4);

	if (buffer)
		return true;

	//delete[] pixels;

	return false;
}

void render(SDL_Window* window) {
	/* FILLED IN LATER */
	if (0 != SDL_LockTexture(buffer, NULL, (void**)&pixels, &pitch))
	{
		cout << "ERROR: Failed to lock texture!" << endl;
		SDL_UnlockTexture(buffer);
		return;
	}

	int pixel_index = 0;
	// RENDER TO PIXELS
	for (int y = 0; y < SCREEN_HEIGHT; y++)
	{
		for (int x = 0; x < SCREEN_WIDTH; x++)
		{
			pixel_index = SCREEN_HEIGHT * y + x;

			pixels[pixel_index] = 127 << 8;
			pixels[pixel_index] |= 127 << 16;
			pixels[pixel_index] |= 127 << 24;
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

/*
int main()
{
	cout << "Hello CMake." << endl;
	return 0;
}

*/
