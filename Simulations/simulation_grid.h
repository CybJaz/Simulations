#pragma once

#include <string>

#include <GL\glew.h>

enum Equation
{
	U = 0,
	V = 1,
	EQUATIONS_NUM = 2
};

class SimulationGrid
{
	const GLsizei GRID_SIZE = 512;

	float _random_base = 10.0f;

	unsigned int _updated_buffer;

	// Equations of dynamic of U and V, that will be copied to GLSL compute shader.
	std::string _dynamic[EQUATIONS_NUM];

	//GLuint _texture_unit[EQUATION_NUM], 
	GLuint _texture_handle[EQUATIONS_NUM];
	GLuint _vao_handle;

	GLuint _compute_program, _render_program;

public:
	SimulationGrid();
	
	void update_grid(float time_step);
	void draw_grid(Equation eq = U);

	void draw_both();

	void reset();

private:
	void init_textures();
	void init_compute_program();
	void init_render_program();

	const GLvoid * gen_red_texture();
	const GLvoid * gen_random_texture(float base);
};
