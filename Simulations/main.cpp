/* Created by: Micha³ Lipiñski 
	lipinski.msl@gmail.com */

/*
TODO:
	+ MAX and MIN value from iteration.
	* reset/stop simulation
	* A and B variable modifiable
	* description - diplay text
	* RK4

	** Extract abstraction. To be possible applying arbitrary rules.
*/

#include <iostream>

#include <GL\glew.h>
#include <glm\glm.hpp>

#include "display.h"
#include "text_2d.h"
#include "simulation_grid.h"

#undef main_

static const int DISPLAY_WIDTH = 512;
static const int DISPLAY_HEIGHT = 512;
static const float FONT_SIZE = 0.05f;

static const float MAX_TIME_PER_FRAME = 1000.f / 45.f;

int main(int argc, char **argv)
{
	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Simulation");
	display.clear(.2f, .4f, 0.7f, 1.0f);

	bool isRunning = true;
	SDL_Event e;

	Text2DGenerator generator("..//common//fonts//consolas.bmp");

	Text2DShader text_shader = generator.generate_shader("shaders//text_shader");

	Text2D txt2d_min_limit = generator.generate_text_2d("", FONT_SIZE, glm::vec2());
	Text2D txt2d_max_limit = generator.generate_text_2d("", FONT_SIZE, glm::vec2());
	// = generator.generate_text_2d("Hello", FONT_SIZE, glm::vec2(-1.f, 1.0f - FONT_SIZE));

	SimulationGrid simulation;

	const Uint8* state = SDL_GetKeyboardState(NULL);

	Equation current_equation = U;

	//Camera2D camera(glm::vec2(), (float)DISPLAY_WIDTH / (float)DISPLAY_HEIGHT, 0.01f);

	bool fullScreen = false;
	while (isRunning)
	{
		display.clear();

		simulation.update_grid(0.01f);
		simulation.draw_grid(current_equation);
		std::string min_limits_str = std::string("Min value: ") + std::to_string(simulation.get_limit(0, current_equation));
		std::string max_limits_str = std::string("Max value: ") + std::to_string(simulation.get_limit(1, current_equation));
		txt2d_min_limit = generator.generate_text_2d(min_limits_str.c_str(), FONT_SIZE, glm::vec2(-1.f, 1.0f - FONT_SIZE));
		txt2d_max_limit = generator.generate_text_2d(max_limits_str.c_str(), FONT_SIZE, glm::vec2(-1.f, 1.0f - 2 * FONT_SIZE));

		text_shader.bind();
		txt2d_min_limit.print();
		txt2d_max_limit.print();
		text_shader.unbind();

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