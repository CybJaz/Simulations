#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>

#include <SDL2/SDL.h>

class Display
{
public:
	Display(unsigned int width, unsigned int height, const std::string& title);

	void clear(float r, float g, float b, float a);
	void clear();
	void swapBuffers();
	void toggleFullScreen();

	float getAspectRatio();

	virtual ~Display();
private:
	void operator=(const Display& display) = delete;
	Display(const Display& display) = delete;

	const std::string _title;
	bool _fullscreen;
	unsigned int _width, _height;
	int _old_pos_w, _old_pos_h;

	SDL_Window* _window;
	SDL_GLContext _glContext;
};

#endif
