#include "opengl.h"
#include <stdio.h>
#include <stdlib.h>

GLuint genComputeProg(GLuint texHandle) {
	// Creating the compute shader, and the program object containing the shader
	GLuint progHandle = glCreateProgram();
	GLuint cs = glCreateShader(GL_COMPUTE_SHADER);

	std::string comp_shader_content = std::string(
		"uniform float roll;\
		layout (rgba16f, binding = 0) uniform image2D destTex;\
		layout (local_size_x = 16, local_size_y = 16) in;\
		void main() {\
			ivec2 storePos = ivec2(gl_GlobalInvocationID.xy);\
			float localCoef = length(vec2(ivec2(gl_LocalInvocationID.xy)-8)/8.0);\
			float globalCoef = sin(float(gl_WorkGroupID.x+gl_WorkGroupID.y)*0.1 + roll)*0.5;\
			float col = imageLoad(destTex, storePos).x;\
			imageStore(destTex, storePos, vec4(") 
			+ (texHandle == 0 ? std::string("1.f-localCoef, globalCoef, 1.0f - col") : std::string("globalCoef, localCoef, col")) 
			+ std::string(", 1.0)); \
		}"
	);

	//float col = mod(roll, 1.0f); \

	// In order to write to a texture, we have to introduce it as image2D.
	// local_size_x/y/z layout variables define the work group size.
	// gl_GlobalInvocationID is a uvec3 variable giving the global ID of the thread,
	// gl_LocalInvocationID is the local index within the work group, and
	// gl_WorkGroupID is the work group's index
	const char *csSrc[] = {
		"#version 430\n",
		comp_shader_content.c_str()
	};
	
	// imageStore(destTex, storePos, vec4(col / 64.f, 0.0, 0.0, 0.0));
// 	imageStore(destTex, storePos, vec4(1.0 - globalCoef*localCoef, 0.0, 0.0, 0.0)); \

	glShaderSource(cs, 2, csSrc, NULL);
	glCompileShader(cs);
	int rvalue;
	glGetShaderiv(cs, GL_COMPILE_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in compiling the compute shader\n");
		GLchar log[10240];
		GLsizei length;
		glGetShaderInfoLog(cs, 10239, &length, log);
		fprintf(stderr, "Compiler log:\n%s\n", log);
		exit(40);
	}
	glAttachShader(progHandle, cs);

	glLinkProgram(progHandle);
	glGetProgramiv(progHandle, GL_LINK_STATUS, &rvalue);
	if (!rvalue) {
		fprintf(stderr, "Error in linking compute shader program\n");
		GLchar log[10240];
		GLsizei length;
		glGetProgramInfoLog(progHandle, 10239, &length, log);
		fprintf(stderr, "Linker log:\n%s\n", log);
		exit(41);
	}
	glUseProgram(progHandle);

	glUniform1i(glGetUniformLocation(progHandle, "destTex"), texHandle);
	//glUniform1i(glGetUniformLocation(progHandle, "destTex"), 0);

	checkErrors("Compute shader");
	return progHandle;
}
