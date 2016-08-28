#include <iostream>

#include "opengl.h"

#include "display.h"

#undef main

GLuint texIdx, texIdx2, tex_handle, tex_handle2;
GLuint renderHandle, renderHandle2, computeHandle, computeHandle2;

GLuint vertexArrayObject, vertexArrayObject2;

void updateTex(int);
void draw();

static const int DISPLAY_WIDTH = 800;
static const int DISPLAY_HEIGHT = 600;

static const float MAX_TIME_PER_FRAME = 1000.f / 45.f;

int main() {

	Display display(DISPLAY_WIDTH, DISPLAY_HEIGHT, "Local");
	display.clear(1.0f, 1.0f, 1.0f, 1.0f);

	texIdx = genTexture(tex_handle);
	texIdx2 = genTexture(tex_handle2);

	std::cout << texIdx << " " << texIdx2 << std::endl;
	std::cout << std::hex << GL_TEXTURE0 << " " << GL_TEXTURE1 << std::endl;
	std::cout << GL_TEXTURE0 + 0 << " " << GL_TEXTURE0 + 1 << std::dec << std::endl;

	renderHandle = genRenderProg(texIdx, vertexArrayObject);
	renderHandle2 = genRenderProg(texIdx2, vertexArrayObject2);
	computeHandle = genComputeProg(texIdx);
	computeHandle2 = genComputeProg(texIdx2);

	std::cout << "rend hand " << renderHandle << " " << renderHandle2 << std::endl;
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, texHandle);

	bool isRunning = true;
	SDL_Event e;

	const Uint8* state = SDL_GetKeyboardState(NULL);
	
	bool fullScreen = false;
	//while (isRunning)
	for (unsigned int i = 0; i < 2048 && isRunning; i++)
	{
		display.clear();

		updateTex(i);
		draw();

		display.swapBuffers();

		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT)
			{
				isRunning = false;
			}
			else if (e.type == SDL_KEYDOWN)
			{
				switch (e.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					isRunning = false;
					break;
				}
			}
		}	
		SDL_Delay(1000 / 60);
	}

		
	return 0;
}

void updateTex(int frame) {
	glUseProgram(computeHandle);
	//glUniform1i(glGetUniformLocation(computeHandle, "destTex"), texIdx);
	//glBindImageTexture(texIdx, tex_handle, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	glUniform1f(glGetUniformLocation(computeHandle, "roll"), (float)frame * 0.01f);
	glDispatchCompute(512/ 16, 512/ 16, 1); // 512^2 threads in blocks of 16^2

	glUseProgram(computeHandle2);
	//glBindImageTexture(texIdx2, texHandle2, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA16F);
	//glUniform1i(glGetUniformLocation(computeHandle2, "destTex"), texIdx2);
	glUniform1f(glGetUniformLocation(computeHandle2, "roll"), (float)frame * 0.005f);
	glDispatchCompute(512 / 16, 512 / 16, 1); // 512^2 threads in blocks of 16^2

	checkErrors("Dispatch compute shader");
}

void draw() {
	glUseProgram(renderHandle);
	glBindVertexArray(vertexArrayObject);
	//glUniform1i(glGetUniformLocation(renderHandle, "srcTex"), texIdx);
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, tex_handle);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	glUseProgram(renderHandle2);
	glBindVertexArray(vertexArrayObject2);
	//glUniform1i(glGetUniformLocation(renderHandle2, "srcTex"), texIdx2);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, tex_handle2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	//swapBuffers();
	checkErrors("Draw screen");
}
