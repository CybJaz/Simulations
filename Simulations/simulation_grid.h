#pragma once

#define _CRT_SECURE_NO_DEPRECATE

#include <string>

#include <GL\glew.h>

static const std::string COMPUTATION_SH_EXT(".compsh");
static const std::string VERTEX_SH_EXT(".vertsh");
static const std::string FRAGMENT_SH_EXT(".fragsh");

enum Equation
{
	U = 0,
	V = 1,
};

class SimulationGrid
{
	const GLsizei GRID_SIZE = 640;
	const GLuint EQUATIONS_NUM = 2;
	const GLuint COEFFICIENTS_NUM = 2;

	float _random_base;
	GLfloat *_limits, *_coeffs;

	unsigned int _updated_buffer;

	GLuint *_texture_handle;
	GLuint _vao_handle;

	GLuint _compute_program, _render_program;

	const std::string _shader_namebase;

	GLuint _limit_values_ssbo, _coeffs_values_ssbo;

public:
	SimulationGrid();
	
	void update_grid(float time_step);
	void draw_grid(Equation eq = U);

	void draw_both();

	void reset();

	//template <Equation EQ>
	inline float get_limit(unsigned int min, Equation eq) const { return _limits[min + 2 * eq]; }

	inline float get_coeff(unsigned int coeff) const { return _coeffs[coeff]; }
	void set_coeff(unsigned int coeff, float value);

	inline float get_random_base() const { return _random_base; }
	inline void set_random_base(float value) { _random_base = value; }

	~SimulationGrid();

private:
	void init_textures();
	void init_compute_program();
	void init_render_program();

	const GLvoid * gen_red_texture();
	const GLvoid * gen_random_texture(float base);
};
