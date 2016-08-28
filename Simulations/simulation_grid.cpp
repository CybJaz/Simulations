#include "simulation_grid.h"

#include <cstdlib>
#include <ctime>

#include "files_utils.h"
#include "opengl_utils.h"

SimulationGrid::SimulationGrid()
{
	_updated_buffer = 0;

	init_textures();
	init_render_program();
	init_compute_program();
}

void SimulationGrid::init_textures()
{
	glGenTextures(EQUATIONS_NUM, &_texture_handle[0]);

	for (unsigned int idx = 0; idx < EQUATIONS_NUM; idx++)
	{
		glActiveTexture(GL_TEXTURE0 + idx);
		glBindTexture(GL_TEXTURE_2D, _texture_handle[idx]);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		//const GLvoid* data = gen_red_texture();
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
	const char *vertex_shader_source[] = {
		"#version 430\n \
		layout(location = 0) in vec2 position;\
		out vec2 texCoord;\
		uniform sampler2D srcTex;\
		uniform uint state;\
		void main() {\
			texCoord = position*0.5f + 0.5f;\
			gl_Position = vec4(position.x, position.y, 0.0, 1.0);\
		}"
	};

	const char *fragment_shader_source[] = {
		"#version 430\n \
		uniform sampler2D srcTex;\
		in vec2 texCoord;\
		uniform uint state;\
		void main() {\
			vec2 oth_coord = vec2(1,1);\
			float sth = texture(srcTex, vec2(0.2,0.4)).x;\
			float col;\
			if (state == 0)\
				col = texture(srcTex, texCoord).x;\
			else\
				col = texture(srcTex, texCoord).y;\
			if(col < 0.0f)\
				gl_FragColor = vec4(0.0f, .6f, 0.4f, 1.0f);\
			else \
				gl_FragColor = vec4(abs(col/6.f), abs(col/6.f) / 3.f, 0.0f, 1.0f);\
		}"
	};

	//if (col >= 0.5f) col = 1.0f; else col = 0.0f; \
	//	gl_FragColor = vec4(col, col, col, 1.0f); \
	//for (unsigned int idx = 0; idx < EQUATIONS_NUM; idx++)
	//{

	// Create render shaders.

		_render_program = glCreateProgram();
		GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
		GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);

		// If NULL at 'length', OpenGL assumes that source is NULL terminated.
		glShaderSource(vsh, 1, vertex_shader_source, NULL);
		glShaderSource(fsh, 1, fragment_shader_source, NULL);

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

		//glUseProgram(_render_program);
		//glUniform1i(glGetUniformLocation(progHandle, "srcTex"), texHandle);

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

		//delete[] vertex_shader_source;
		//delete fragment_shader_source; 
}

void SimulationGrid::init_compute_program()
{
	//std::string comp_shader_content = load_txt_file("shaders\\loc_comp_shader.csh");
	//std::string comp_shader_content = load_txt_file("shaders\\gof_comp_shader.csh");
	std::string comp_shader_content = load_txt_file("shaders\\turing_comp_shader.csh");

	std::string comp_shader_content_wait = std::string(
		"#version 430\n \
		uniform int upd_buf;\n\
		layout (rg32f, binding = 0) uniform image2D u_vals;\n\
		layout (rg32f, binding = 1) uniform image2D v_vals;\n\
		layout (local_size_x = 1, local_size_y = 1) in;\n\
		\
		uint modui(int x, int n) { if(x < 0) return n+x; else if(x>=n) return x-n; else return x;}\
		\
		void main() {\n\
			float A = 3.5, B = 16;\n\
			int n = 1024;\n\
			float time_step = 0.001f;\n\
			ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);\n\
			int xc = storePos.x, yc = storePos.y;\n\
			uint r_buf = 1 - upd_buf;\n\
			vec2 u_values = imageLoad(u_vals, storePos).xy;\n\
			vec2 v_values = imageLoad(v_vals, storePos).xy;\n\
			float curr_u = u_values[r_buf];\n\
			float curr_v = v_values[r_buf];\n\
			uint xd = modui(xc-1, n), xu = modui(xc+1, n), yd = modui(yc-1, n), yu = modui(yc+1, n);\n\
			float kernel_u = 8 * curr_u \
				- (imageLoad(u_vals, ivec2(xd, storePos.y))[r_buf] + imageLoad(u_vals, ivec2(xu, storePos.y))[r_buf] + imageLoad(u_vals, ivec2(storePos.x, yd))[r_buf] + imageLoad(u_vals, ivec2(storePos.x, yu))[r_buf])\
				-(imageLoad(u_vals, ivec2(xd, yd))[r_buf] + imageLoad(u_vals, ivec2(xu, yd))[r_buf] + imageLoad(u_vals, ivec2(xd, yu))[r_buf] + imageLoad(u_vals, ivec2(xu, yu))[r_buf]); \n\
			\
			float kernel_v = 8 * curr_v \
				- (imageLoad(v_vals, ivec2(xd, storePos.y))[r_buf] + imageLoad(v_vals, ivec2(xu, storePos.y))[r_buf] + imageLoad(v_vals, ivec2(storePos.x, yd))[r_buf] + imageLoad(v_vals, ivec2(storePos.x, yu))[r_buf])\
				-(imageLoad(v_vals, ivec2(xd, yd))[r_buf] + imageLoad(v_vals, ivec2(xu, yd))[r_buf] + imageLoad(v_vals, ivec2(xd, yu))[r_buf] + imageLoad(v_vals, ivec2(xu, yu))[r_buf]); \n\
			\
			float d_u = curr_u * (curr_v - 1) - 12 - A * kernel_u;\n\
			float d_v = 16 - curr_u * curr_v - B * kernel_v;\n\
			\
			float new_u = curr_u + time_step * d_u;\n \
			float new_v = curr_v + time_step * d_v;\n \
			if(upd_buf == 0){ \n\
				imageStore(u_vals, storePos, vec4(new_u, u_values.y, 0.f, 0.f));\n\
				imageStore(v_vals, storePos, vec4(new_v, v_values.y, 0.f, 0.f));\n\
			} else { \n\
				imageStore(u_vals, storePos, vec4(u_values.x, new_u, 0.f, 0.f));\n\
				imageStore(v_vals, storePos, vec4(v_values.x, new_v, 0.f, 0.f));\n\
			} \n\
		}"
	);
	// float new_u_val = mod(u_values[1-upd_buf] + 0.02f, 1.0f); \n\

	//float ycol = mod(imageLoad(v_vals, storePos).y + 0.02f, 1.0f); \
	//imageStore(v_vals, storePos, vec4(xcol, ycol, 0.f, 0.0f)); \

	const char * comp_shader_source[] = { comp_shader_content.c_str() };

	_compute_program = glCreateProgram();
	GLuint csh = glCreateShader(GL_COMPUTE_SHADER);

	glShaderSource(csh, 1, comp_shader_source, NULL);
	glCompileShader(csh);
	checkShaderError(csh, GL_COMPILE_STATUS, false, "Error compiling shader!");

	glAttachShader(_compute_program, csh);

	glLinkProgram(_compute_program);
	checkShaderError(_render_program, GL_LINK_STATUS, true, "Error linking shader program");

	glUseProgram(_compute_program);

	glUniform1i(glGetUniformLocation(_compute_program, "u_vals"), 0);
	glUniform1i(glGetUniformLocation(_compute_program, "v_vals"), 1);
	//glUniform1i(glGetUniformLocation(progHandle, "destTex"), 0);

	checkErrors("Compute shader");

}

void SimulationGrid::reset()
{

}

void SimulationGrid::update_grid(float time_step)
{
	_updated_buffer = 1 - _updated_buffer;
	glUseProgram(_compute_program);
	//glBindImageTexture(0, _texture_handle[U], 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glUniform1i(glGetUniformLocation(_compute_program, "u_vals"), 0);
	glUniform1i(glGetUniformLocation(_compute_program, "v_vals"), 1);
	glUniform1i(glGetUniformLocation(_compute_program, "upd_buf"), _updated_buffer);
	//glUniform1f(glGetUniformLocation(computeHandle, "roll"), (float)frame * 0.01f);

	GLuint local_groups = 1;
	glDispatchCompute(GRID_SIZE / local_groups, GRID_SIZE / local_groups, 1); // 512^2 threads in blocks of 16^2s
}

void SimulationGrid::draw_grid(Equation eq)
{
	glUseProgram(_render_program);
	glUniform1i(glGetUniformLocation(_render_program, "srcTex"), eq);
	glUniform1i(glGetUniformLocation(_compute_program, "state"), _updated_buffer);
	glBindVertexArray(_vao_handle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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