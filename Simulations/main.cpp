/* Created by: Micha³ Lipiñski 
	lipinski.msl@gmail.com */

/*
TODO:
	* MAX and MIN value from iteration.
	* reset/stop simulation
	* A and B variable modifiable
	* description
*/

#include <iostream>

#include <GL\glew.h>
#include <glm\glm.hpp>

#include "display.h"
#include "simulation_grid.h"

#undef main_

static const int DISPLAY_WIDTH = 512;
static const int DISPLAY_HEIGHT = 512;

static const float MAX_TIME_PER_FRAME = 1000.f / 45.f;

int main(int argc, char **argv)
{
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Turing gepard");
	display.clear(.2f, .4f, 0.7f, 1.0f);

	bool isRunning = true;
	SDL_Event e;

	SimulationGrid simulation;

	const Uint8* state = SDL_GetKeyboardState(NULL);

	//Camera2D camera(glm::vec2(), (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.01f);

	bool fullScreen = false;
	while (isRunning)
	{

		display.clear();

		simulation.update_grid(0.01f);
		simulation.draw_grid(V);

		display.swapBuffers();

		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				isRunning = false;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				}
			}
		}


		SDL_Delay(1000 / 60);
	}

	return 0;
}