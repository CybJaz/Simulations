#include "display.h"

#include <iostream>
#include <GL/glew.h>


Display::Display(unsigned int width, unsigned int height, const std::string& title)
	: _title(title), _fullscreen(false), _width(width), _height(height)
{
	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);

	SDL_GL_SetSwapInterval(0);

	//window_ = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
	//	width, height, SDL_WINDOW_OPENGL);
	_window = SDL_CreateWindow(_title.c_str(),
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		_width, _height,
		SDL_WINDOW_OPENGL);

	_glContext = SDL_GL_CreateContext(_window);

	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		std::cerr << "Glew failed to initialize!" << std::endl;
	}

	//glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	//glDisable(GL_CULL_FACE);
	//glEnable(GL_CULL_FACE);

	//glCullFace(GL_BACK);

	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//float att[3] = { 1.0f, 1.0f, 1.0f };
	//glEnable(GL_POINT_SPRITE);
	//glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
	//glPointParameterf(GL_POINT_SIZE_MIN, 1.0f);
	//glPointParameterf(GL_POINT_SIZE_MAX, 16.0f);
	//glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, att);
	//glEnable(GL_POINT_SPRITE);

}

Display::~Display()
{
	SDL_GL_DeleteContext(_glContext);
	SDL_DestroyWindow(_window);
	SDL_Quit();
}

void Display::clear(float r, float g, float b, float a)
{
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Display::swapBuffers()
{
	SDL_GL_SwapWindow(_window);
}

void Display::toggleFullScreen()
{
	if (_fullscreen)
	{
		SDL_SetWindowBordered(_window, SDL_TRUE);
		SDL_SetWindowSize(_window, _width, _height);
		SDL_SetWindowPosition(_window, _old_pos_w, _old_pos_h);

		glViewport(0, 0, (GLsizei)_width, (GLsizei)_height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	}
	else
	{
		SDL_GetWindowPosition(_window, &_old_pos_w, &_old_pos_h);
		SDL_SetWindowBordered(_window, SDL_FALSE);
		int d = SDL_GetWindowDisplayIndex(_window);
		SDL_DisplayMode current;
		int should_be_zero = SDL_GetCurrentDisplayMode(d, &current);
		SDL_SetWindowSize(_window, current.w, current.h);
		SDL_SetWindowPosition(_window, 0, 0);

		glViewport(0, 0, (GLsizei)current.w, (GLsizei)current.h);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
	}

	_fullscreen = !_fullscreen;
}

float Display::getAspectRatio()
{
	if (_fullscreen)
	{
		int d = SDL_GetWindowDisplayIndex(_window);
		SDL_DisplayMode current;
		int should_be_zero = SDL_GetCurrentDisplayMode(d, &current);
		return (float)current.w / (float)current.h;
	}
	else
	{
		return (float)_width / (float)_height;
	}
}