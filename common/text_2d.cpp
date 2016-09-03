#include "text_2d.h"

#include "opengl_utils.h"

std::ostream& operator <<(std::ostream& stream, const glm::vec2 vec)
{
	stream << "[" << vec.x << ", " << vec.y << "]";
	return stream;
}

Text2D::Text2D(GLuint vertex_buffer_id, GLuint uv_buffer_id, GLuint num_of_vertices)
	: _vertex_buffer_id(vertex_buffer_id), _uv_buffer_id(uv_buffer_id),
	_num_of_vertices(num_of_vertices)
{}

void Text2D::print()
{
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, _vertex_buffer_id);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, _uv_buffer_id);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//// Draw call
	glDrawArrays(GL_TRIANGLES, 0, _num_of_vertices);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
}

Text2DShader::Text2DShader(const char * namebase, GLuint texture_buffer_id)
	:_font_texture_buffer_id(texture_buffer_id)
{
	std::string vertex_shader_content = load_txt_file(std::string(namebase) + std::string(".vsh"));
	GLint vlengths[1] = { vertex_shader_content.length() };
	const char *vertex_shader_source[] = { vertex_shader_content.c_str() };

	std::string fragment_shader_content = load_txt_file(std::string(namebase) + std::string(".fsh"));
	GLint flengths[1] = { fragment_shader_content.length() };
	const char *fragment_shader_source[] = { fragment_shader_content.c_str() };

	// Create render shaders.

	_program = glCreateProgram();
	GLuint vsh = glCreateShader(GL_VERTEX_SHADER);
	GLuint fsh = glCreateShader(GL_FRAGMENT_SHADER);

	// If NULL at 'length', OpenGL assumes that source is NULL terminated.
	glShaderSource(vsh, 1, vertex_shader_source, vlengths);
	glShaderSource(fsh, 1, fragment_shader_source, flengths);

	glCompileShader(vsh);
	checkShaderError(vsh, GL_COMPILE_STATUS, false, "Error compiling shader!");

	glCompileShader(fsh);
	checkShaderError(fsh, GL_COMPILE_STATUS, false, "Error compiling shader!");

	glAttachShader(_program, vsh);
	glAttachShader(_program, fsh);

	glBindAttribLocation(_program, 0, "position_screenspace");
	glBindAttribLocation(_program, 1, "vertex_uv");

	glLinkProgram(_program);
	checkShaderError(_program, GL_LINK_STATUS, true, "Error linking shader program");

	glValidateProgram(_program);
	checkShaderError(_program, GL_LINK_STATUS, true, "Invalid shader program");
	checkErrors("Render shader");

}

void Text2DShader::bind()
{
	glUseProgram(_program);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _font_texture_buffer_id);
	// Set our "myTextureSampler" sampler to user Texture Unit 0
	glUniform1i(glGetUniformLocation(_program, "font_texture"), 0);
}

void Text2DShader::unbind()
{
	glDisable(GL_BLEND);
}

Text2DGenerator::Text2DGenerator(const char * texturePath)
{
	// Initialize texture
	//Text2DTextureID = loadDDS(texturePath);

	_bitmap = new BitMap(texturePath);

	// Create one OpenGL texture
	glGenTextures(1, &_texture_buffer_id);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, _texture_buffer_id);

	// Give the image to OpenGL
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _bitmap->width, _bitmap->height, 0, GL_BGR, GL_UNSIGNED_BYTE, _bitmap->data);


	// TODO: usun¹æ?
	// OpenGL has now copied the data. Free our own version
	// delete[] data;

	// Poor filtering, or ...
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 

	// ... nice trilinear filtering.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Initialize VBO
	//glGenBuffers(1, &Text2DVertexBufferID);
	//glGenBuffers(1, &Text2DUVBufferID);

	// Initialize Shader
	//Text2DShaderID = LoadShaders("TextVertexShader.vertexshader", "TextVertexShader.fragmentshader");

	// Initialize uniforms' IDs
	//Text2DUniformID = glGetUniformLocation(Text2DShaderID, "myTextureSampler");
}

Text2DGenerator::~Text2DGenerator()
{
}

Text2D Text2DGenerator::generate_text_2d(const char * text, float size, glm::vec2 position)
{
	unsigned int length = strlen(text);

	// Fill buffers
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;
	for (unsigned int i = 0; i<length; i++){

		glm::vec2 vertex_up_left = glm::vec2(i * size / 2, size) + position;
		glm::vec2 vertex_up_right = glm::vec2(i * size / 2 + size, size) + position;
		glm::vec2 vertex_down_right = glm::vec2(i * size / 2 + size, 0) + position;
		glm::vec2 vertex_down_left = glm::vec2(i * size / 2, 0) + position;

		//std::cout << "V: " << vertex_down_left << " " << vertex_down_right << " " <<
		//	vertex_up_left << " " << vertex_up_right << std::endl;

		vertices.push_back(vertex_up_left);
		vertices.push_back(vertex_down_left);
		vertices.push_back(vertex_up_right);

		vertices.push_back(vertex_down_right);
		vertices.push_back(vertex_up_right);
		vertices.push_back(vertex_down_left);

		char character = text[i];
		float uv_x = (character % 16) / 16.0f;
		float uv_y = 1.0f - (character / 16) / 16.0f;

		//std::cout << character << ": " << uv_x << " " << uv_y << std::endl;
		
		glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		glm::vec2 uv_down_left = glm::vec2(uv_x, uv_y - 1.0f / 16.0f);
		glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y - 1.0f / 16.0f);

		//glm::vec2 uv_up_left = glm::vec2(0, 0);
		//glm::vec2 uv_up_right = glm::vec2(0 + 1.0f / 16.0f, 0);
		//glm::vec2 uv_down_right = glm::vec2(0 + 1.0f / 16.0f, (0 + 1.0f / 16.0f));
		//glm::vec2 uv_down_left = glm::vec2(0, (0 + 1.0f / 16.0f));

		//glm::vec2 uv_up_left = glm::vec2(uv_x, uv_y);
		//glm::vec2 uv_up_right = glm::vec2(uv_x + 1.0f / 16.0f, uv_y);
		//glm::vec2 uv_down_right = glm::vec2(uv_x + 1.0f / 16.0f, (uv_y + 1.0f / 16.0f));
		//glm::vec2 uv_down_left = glm::vec2(uv_x, (uv_y + 1.0f / 16.0f));
		UVs.push_back(uv_up_left);
		UVs.push_back(uv_down_left);
		UVs.push_back(uv_up_right);

		UVs.push_back(uv_down_right);
		UVs.push_back(uv_up_right);
		UVs.push_back(uv_down_left);

		//std::cout << "U: " << uv_down_left << " " << uv_down_right << " " <<
		//	uv_up_left << " " << uv_up_right << std::endl;

	}

	GLuint text2d_vertex_buffer_id, text2d_uv_buffer_id;

	glGenBuffers(1, &text2d_vertex_buffer_id);
	glGenBuffers(1, &text2d_uv_buffer_id);

	glBindBuffer(GL_ARRAY_BUFFER, text2d_vertex_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, text2d_uv_buffer_id);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	return Text2D(text2d_vertex_buffer_id, text2d_uv_buffer_id, vertices.size());

	// Bind shader
	//glUseProgram(Text2DShaderID);

	//// Bind texture
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, _texture_buffer_id);
	//// Set our "myTextureSampler" sampler to user Texture Unit 0
	//glUniform1i(Text2DUniformID, 0);

	//// 1rst attribute buffer : vertices
	//glEnableVertexAttribArray(0);
	//glBindBuffer(GL_ARRAY_BUFFER, Text2DVertexBufferID);
	//glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	//// 2nd attribute buffer : UVs
	//glEnableVertexAttribArray(1);
	//glBindBuffer(GL_ARRAY_BUFFER, Text2DUVBufferID);
	//glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

}

Text2DShader Text2DGenerator::generate_shader(const char * shader_namebase)
{
	return Text2DShader(shader_namebase, _texture_buffer_id);
}
