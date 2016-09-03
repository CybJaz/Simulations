#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include <string>

#include <GL\glew.h>

static const std::string COMPUTATION_SH_EXT(".csh");
static const std::string VERTEX_SH_EXT(".vsh");
static const std::string FRAGMENT_SH_EXT(".fsh");

enum Equation
{
	U = 0,
	V = 1,
};

class SimulationGrid
{
	const GLsizei GRID_SIZE = 128;
	const GLuint EQUATIONS_NUM = 2;

	float _random_base;
	GLfloat * _limits;

	unsigned int _updated_buffer;

	// Equations of dynamic of U and V, that will be copied to GLSL compute shader.
	//std::string _dynamic[EQUATIONS_NUM];

	GLuint *_texture_handle;
	GLuint _vao_handle;

	GLuint _compute_program, _render_program;

public:
	SimulationGrid();
	
	void update_grid(float time_step);
	void draw_grid(Equation eq = U);

	void draw_both();

	void reset();

	//template <Equation EQ>
	float get_limit(unsigned int min, Equation eq) { return _limits[min + 2 * eq]; }

	~SimulationGrid();

private:
	void init_textures();
	void init_compute_program();
	void init_render_program();

	const GLvoid * gen_red_texture();
	const GLvoid * gen_random_texture(float base);

	const std::string _shader_namebase;

	GLuint _limit_values_ssbo;
};
