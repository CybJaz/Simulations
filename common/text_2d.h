#ifndef _TEXT_2D_H
#define _TEXT_2D_H

#define _CRT_SECURE_NO_DEPRECATE

#include <string>
#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>

#include "files_utils.h"
#include "transform_2d.h"

class Text2D
{
public:
	void print();

	Transform2D transform;

private:
	Text2D();
	Text2D(GLuint vertex_buffer_id, GLuint uv_buffer_id, GLuint num_of_vertices);

	GLuint _vertex_buffer_id,
		_uv_buffer_id;

	GLuint _num_of_vertices;

	friend class Text2DGenerator;
};

class Text2DShader
{
private:
	GLuint _program;
	GLuint _font_texture_buffer_id;

	Text2DShader(const char * namebase, GLuint texture_buffer_id);
	friend class Text2DGenerator;

public:

	void bind();
	void unbind();
};

class Text2DGenerator
{
	BitMap * _bitmap;
	GLuint _texture_buffer_id;
public:
	Text2DGenerator(const char * texturePath);
	~Text2DGenerator();

	//Text2D generate_text_2d(const std::string& text, int size);
	/*
	\param text 
	\param size relative to screensize
	*/
	Text2D generate_text_2d(const char * text, float size, glm::vec2 position);

	Text2DShader generate_shader(const char * shader_namebase);

	// void printText2D(const char * text, int x, int y, int size);
	void cleanupText2D();
};

#endif // _TEXT_2D_H