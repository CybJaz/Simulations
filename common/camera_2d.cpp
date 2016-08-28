#include "camera_2d.h"

#include "glm_printer.h"

#include <glm\gtc\matrix_transform.hpp>

Camera2D::Camera2D(const glm::vec2& position, float aspect, float speed, float height)
{
	_position = position;
	_speed = speed;

	float width = aspect * height;

	float right = width / 2.0f;
	float left = -right;
	float top = height / 2.0f;
	float bottom = - top;
	
	_projection = glm::ortho(left, right, bottom, top);

	_projection3 = glm::mat3();
	_projection3[0][0] = 2.0f / width;
	_projection3[1][1] = 2.0f / height;

	_projection3[2][1] = - (top + bottom) / (top - bottom);
	_projection3[2][0] = - (right + left) / (right - left);

}
