#ifndef GLM_PRINTER_H
#define GLM_PRINTER_H

#include <iostream>

#include <glm\glm.hpp>

inline void print_mat4(glm::mat4 mat)
{
	for (unsigned int i = 0; i < 4; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			std::cout << mat[j][i] << " ";
		}
		std::cout << std::endl;
	}
}

inline void print_mat3(glm::mat3 mat)
{
	for (unsigned int i = 0; i < 3; i++)
	{
		for (unsigned int j = 0; j < 3; j++)
		{
			std::cout << mat[j][i] << " ";
		}
		std::cout << std::endl;
	}
}

#endif