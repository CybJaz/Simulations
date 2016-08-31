/* Created by Michał Lipiński
	Compute shader for Turing patterns.

	Shader write and read from the same image, but in one iteration it uses 
	red layer to read and green to write and vice versa.
*/

#version 430

uniform int upd_buf;

layout (std430, binding = 0) buffer limits
{
	float minmax[];
};

layout (local_size_x = 1, local_size_y = 1) in;

void main() {
	minmax[0] = 1.5f;
	minmax[1] = 1.5f;
}
