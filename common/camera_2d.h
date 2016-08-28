#ifndef CAMERA_H
#define CAMERA_H

#include <glm\glm.hpp>
//#include <glm\gtc\quaternion.hpp>
//#include <glm\gtx\transform.hpp>

class Camera2D
{
public:
	/**
	* Camera constructor.
	* \param position 2D vector
	* \param aspect Width to Height ratio.
	* \param height Distance from bottom to top.
	*/
	Camera2D(const glm::vec2& position, float aspect, float speed = 0.1f,  float height = 2.0f);

	inline glm::mat4 get_projection() const
	{
		return _projection;
	}

	inline glm::mat3 get_projection3() const
	{
		return _projection3; 
	}

	inline glm::mat3 get_view_projection3() const
	{
		return _projection3;
	}

	inline glm::mat4 get_view_projection() const
	{
		//glm::vec3 pos = _spinor.rotate(_position);
		//glm::vec3 forw = _spinor.rotate(_forward);
		//glm::vec3 up = _spinor.rotate(_up);
		//glm::mat4 lookat = glm::lookAt(_position, _position + forw, up);

		return _projection * get_view();
	}

	inline glm::mat4 get_view() const
	{
		glm::mat4 view = glm::mat4();
		view[3][0] = -_position[0];
		view[3][1] = -_position[1];
		return view;
	}

	inline glm::mat3 get_view3() const
	{
		glm::mat3 view = glm::mat3();
		view[2][0] = -_position[0];
		view[2][1] = -_position[1];
		return view;
	}

	//inline glm::mat4 getView() const
	//{
	//	//glm::vec3 pos = _spinor.rotate(_position);
	//	glm::vec3 forw = _spinor.rotate(_forward);
	//	glm::vec3 up = _spinor.rotate(_up);
	//	glm::mat4 lookat = glm::lookAt(_position, _position + forw, up);
	//	return lookat;
	//}

	inline void set_position(glm::vec2 pos) { _position = pos; }
	inline glm::vec2 get_position() const { return _position; }

	inline void set_speed(float speed) { _speed = speed; }

	inline void move(glm::vec2 vec) { _position += _speed * vec; }

private:
	Camera2D(const Camera2D&) = delete;
	Camera2D & operator=(const Camera2D&) = delete;

	glm::mat4 _projection;
	glm::mat3 _projection3;

	float _rotation;

	glm::vec2 _position;

	float _speed;
};

#endif
