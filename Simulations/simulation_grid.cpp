#include "simulation_grid.h"

#include <cstdlib>
#include <ctime>
#include <utility>

#include "files_utils.h"
#include "opengl_utils.h"

SimulationGrid::SimulationGrid()
	:_shader_namebase("shaders\\turing_shader")
{
	_updated_buffer = 0;
	_random_base = 10.f;

	_texture_handle = new GLuint[EQUATIONS_NUM];

	_coeffs = new GLfloat[COEFFICIENTS_NUM];
	_coeffs[0] = 3.5f;
	_coeffs[1] = 16.f;

	_limits = new GLfloat[2 * EQUATIONS_NUM];
	_limits[0] = _random_base / 2.0f;
	_limits[1] = _random_base / 4.0f;
	_limits[2] = _random_base / 2.0f;
	_limits[3] = _random_base / 4.0f;

	glGenBuffers(1, &_limit_values_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _limit_values_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * 2 * EQUATIONS_NUM, _limits, GL_DYNAMIC_DRAW);
	//GLvoid* p = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
	//memcpy(p, &_limits, sizeof(_limits));
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	glGenBuffers(1, &_coeffs_values_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _coeffs_values_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * COEFFICIENTS_NUM, _coeffs, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	init_textures();
	init_render_program();
	init_compute_program();

	//delete[] _limits;
}


SimulationGrid::~SimulationGrid()
{
	// ???
	//glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	//glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	//delete[] _limits;
	//delete[] _texture_handle;

	for (unsigned int idx = 0; idx < EQUATIONS_NUM; idx++)
	{
		glDeleteTextures(EQUATIONS_NUM, &_texture_handle[0]);
	}
}

void SimulationGrid::init_textures()
{
	glGenTextures(EQUATIONS_NUM, &_texture_handle[0]);

	for (unsigned int idx = 0; idx < EQUATIONS_NUM; idx++)
	{
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_2D, _texture_handle[idx]);
		checkErrors("bind texture");

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		checkErrors("parameters texture");

		const GLvoid* data = gen_random_texture(_random_base);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RG32F, GRID_SIZE, GRID_SIZE, 0, GL_RG, GL_FLOAT, data);
		checkErrors("Gen texture");

		glBindImageTexture(idx, _texture_handle[idx], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
		checkErrors("Gen image");

		delete[] (GLfloat*) data;
	}
}

void SimulationGrid::init_render_program()
{
	std::string vertex_shader_content = load_txt_file(_shader_namebase+VERTEX_SH_EXT);
	GLint vlengths[1] = { vertex_shader_content.length() };
	const char *vertex_shader_source[] = { vertex_shader_content.c_str() };

	std::string fragment_shader_content = load_txt_file(_shader_namebase + FRAGMENT_SH_EXT);
	GLint flengths[1] = { fragment_shader_content.length() };
	const char *fragment_shader_source[] = { fragment_shader_content.c_str() };

	// Create render shaders.

		_render_program = glCreateProgram();
		GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
		GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);

		// If NULL at 'length', OpenGL assumes that source is NULL terminated.
		glShaderSource(vsh, 1, vertex_shader_source, vlengths);
		glShaderSource(fsh, 1, fragment_shader_source, flengths);

		glCompileShader(vsh);
		checkShaderError(vsh, GL_COMPILE_STATUS, false, "Error compiling shader!");

		glCompileShader(fsh);
		checkShaderError(fsh, GL_COMPILE_STATUS, false, "Error compiling shader!");

		glAttachShader(_render_program, vsh);
		glAttachShader(_render_program, fsh);

		glBindAttribLocation(_render_program, 0, "position");

		glLinkProgram(_render_program);
		checkShaderError(_render_program, GL_LINK_STATUS, true, "Error linking shader program");

		glValidateProgram(_render_program);
		checkShaderError(_render_program, GL_LINK_STATUS, true, "Invalid shader program");
		checkErrors("Render shader");

	// Create "mesh".
	float data[] = {
		-1.0f, -1.0f,
		-1.0f, 1.0f,
		1.f, -1.0f,
		1.f, 1.0f };
		
	glGenVertexArrays(1, &_vao_handle);
	glBindVertexArray(_vao_handle);

	GLuint position_buffer;
	glGenBuffers(1, &position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, data, GL_STREAM_DRAW);

	GLint position_loc = glGetAttribLocation(_render_program, "position");
	glVertexAttribPointer(position_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(position_loc);

	checkErrors("Square mesh");

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void SimulationGrid::init_compute_program()
{
	//std::string comp_shader_content = load_txt_file("shaders\\loc_comp_shader.csh");
	//std::string comp_shader_content = load_txt_file("shaders\\gof_comp_shader.csh");
	std::string comp_shader_content = load_txt_file(_shader_namebase + COMPUTATION_SH_EXT);
	GLint lengths[1] = { comp_shader_content.length() };

	const char * comp_shader_source[] = { comp_shader_content.c_str() };

	_compute_program = glCreateProgram();
	GLuint csh = glCreateShader(GL_COMPUTE_SHADER);

	glShaderSource(csh, 1, comp_shader_source, lengths);
	glCompileShader(csh);
	checkShaderError(csh, GL_COMPILE_STATUS, false, "Error compiling shader!");

	glAttachShader(_compute_program, csh);

	glLinkProgram(_compute_program);
	checkShaderError(_render_program, GL_LINK_STATUS, true, "Error linking shader program");

	glUseProgram(_compute_program);
	checkErrors("Compute shader");

	glUniform1i(glGetUniformLocation(_compute_program, "u_vals"), 0);
	glUniform1i(glGetUniformLocation(_compute_program, "v_vals"), 1);
	
	GLint x;
	glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &x);

	glUseProgram(_compute_program);
	{
		GLuint ssbo_binding_point_index = 2;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index, _limit_values_ssbo);
		checkErrors("SSBO1");

		GLuint block_index = 0;
		std::cout << "max gl uint: " << block_index - 1 << " block index: " << block_index << std::endl;
		block_index = glGetProgramResourceIndex(_compute_program, GL_SHADER_STORAGE_BLOCK, "limits");
		checkErrors("SSBO");

		std::cout << "block index: " << block_index << " GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS: " << x << std::endl;

		glShaderStorageBlockBinding(_compute_program, block_index, ssbo_binding_point_index);
		checkErrors("SSBO1.1");
	}
	
	///
	{
		GLuint ssbo_binding_point_index = 3;
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index, _coeffs_values_ssbo);
		checkErrors("SSBO2");

		GLuint block_index = 0;
		std::cout << "max gl uint: " << block_index - 1 << " block index: " << block_index << std::endl;
		block_index = glGetProgramResourceIndex(_compute_program, GL_SHADER_STORAGE_BLOCK, "coeffs");
		checkErrors("SSBO2.1");

		std::cout << "block index: " << block_index << " GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS: " << x << std::endl;

		glShaderStorageBlockBinding(_compute_program, block_index, ssbo_binding_point_index);
		checkErrors("SSBO2.2");
	}

	//glBindBufferRange(GL_SHADER_STORAGE_BUFFER, ssbo_binding_point_index, _limit_values_ssbo, 0, sizeof(GLfloat) * 2);
	glUseProgram(0);

}

void SimulationGrid::update_grid(float time_step)
{
	_updated_buffer = 1 - _updated_buffer;
	glUseProgram(_compute_program);
	glUniform1i(glGetUniformLocation(_compute_program, "u_vals"), 0);
	glUniform1i(glGetUniformLocation(_compute_program, "v_vals"), 1);
	glUniform1i(glGetUniformLocation(_compute_program, "upd_buf"), _updated_buffer);

	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _limit_values_ssbo);
	//glShaderStorageBlockBinding(_compute_program, glGetProgramResourceIndex(_compute_program, GL_SHADER_STORAGE_BLOCK, "limits"), 2);
	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, _coeffs_values_ssbo);

	for (unsigned int i = 0; i < EQUATIONS_NUM; i++)
	{
		std::swap(_limits[2 * i], _limits[2 * i + 1]);

		//glActiveTexture(GL_TEXTURE0 + i);
		//glBindTexture(GL_TEXTURE_2D, _texture_handle[i]);
		//glBindImageTexture(i, _texture_handle[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RG32F);
	}

	GLuint local_groups = 1;
	glDispatchCompute(GRID_SIZE / local_groups, GRID_SIZE / local_groups, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	//glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, _limit_values_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _limit_values_ssbo);
	_limits = (GLfloat *) glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	//lim = (GLfloat *) glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLfloat) * 2, GL_MAP_READ_BIT);

	// Reading the buffer
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}

void SimulationGrid::draw_grid(Equation eq)
{
	glUseProgram(_render_program);
	//glUniform1i(glGetUniformLocation(_render_program, "srcTex"), eq);
	glUniform1i(glGetUniformLocation(_render_program, "state"), _updated_buffer);
	//std::cout << _limits[0] << " : " << _limits[1] << " " << _limits[2] << " " << _limits[3] << std::endl;
	glUniform2f(glGetUniformLocation(_render_program, "limits"), _limits[2 * eq], _limits[2 * eq + 1]);

	glActiveTexture(GL_TEXTURE0 + eq);
	glBindTexture(GL_TEXTURE_2D, _texture_handle[eq]);
	glUniform1i(glGetUniformLocation(_render_program, "srcTex"), eq);

	glEnableVertexAttribArray(0);
	glBindVertexArray(_vao_handle);
	//glBindBuffer(GL_ARRAY_BUFFER, _vao_handle);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glBindVertexArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
}

void SimulationGrid::set_coeff(unsigned int coeff, float value)
{
	_coeffs[coeff] = value;

	//glGenBuffers(1, &_coeffs_values_ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, _coeffs_values_ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(GLfloat) * COEFFICIENTS_NUM, _coeffs, GL_DYNAMIC_DRAW);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

}

void SimulationGrid::reset()
{
	for (unsigned int idx = 0; idx < EQUATIONS_NUM; idx++)
	{
		glBindTexture(GL_TEXTURE_2D, _texture_handle[idx]);
		const GLvoid* data = gen_random_texture(_random_base);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, GRID_SIZE, GRID_SIZE, GL_RG, GL_FLOAT, data);
	}
}

const GLvoid * SimulationGrid::gen_red_texture()
{
	GLfloat* data = new GLfloat[GRID_SIZE * GRID_SIZE * 2];

	for (int x = 0; x < GRID_SIZE * GRID_SIZE * 2; x += 2)
	{
		data[x + 0] = 1.0f;
		data[x + 1] = .0f;
	}

	return (GLvoid*) data;
}

const GLvoid * SimulationGrid::gen_random_texture(float base)
{
	GLfloat* data = new GLfloat[GRID_SIZE * GRID_SIZE * 2];
	std::srand((unsigned)std::time(0));
	for (int x = 0; x < GRID_SIZE * GRID_SIZE * 2; x++)
		data[x] = ((float) std::rand() / RAND_MAX) * base;

	return (GLvoid*)data;
}