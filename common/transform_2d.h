#ifndef TRANSFORM_INCLUDED_H
#define TRANSFORM_INCLUDED_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "camera_2d.h"

struct Transform2D
{
public:
	/**
	Transform struct constructor.	
	\param pos translation vector.
	\param rot rotation angle in radians.
	\param scale scale among each axis, default (1,1).
	\note Forward vector is (0,0,1), up vector is (0,1,0).
	*/
	Transform2D(const glm::vec2& pos = glm::vec2(), const float& angle = 0.0f, 
		const glm::vec2& scale = glm::vec2(1.0f, 1.0f));

	/**
	Creates Trasnform struct only with rotation.
	\param rotation RPY vector (roll, pitch, yaw).
	*/
	static Transform2D rotation(float& angle);
	/**
	Creates Trasnform struct only with scale.
	*/
	static Transform2D scale(glm::vec2& scale);
	/**
	Creates Trasnform struct only with translation.
	*/
	static Transform2D translation(glm::vec2& translation);

	/**
	Transforms given point.
	*/
	glm::vec2 operator () (const glm::vec2& point) const;

	/**
	Rotate given point.
	*/
	glm::vec2 rotatePoint(const glm::vec2& point) const;

	/**
	Transform point
	*/
	glm::vec2 transformPoint(const glm::vec2& point) const;


	/**
	Generates Model transformation matrix. I.e. translation + scaling + rotation.
	*/
	glm::mat4 getModel() const;

	///**
	//Generates Rotation matrix.
	//*/
	//glm::mat4 getRotationMat4() const;

	//glm::mat3 getRotationMat3() const;

	/**
	Generates Model-View-Projection matrix.
	*/
	glm::mat4 getMVP(const Camera2D& camera) const;
	
	glm::mat3 getMVP3(const Camera2D& camera) const;

	/**
	Generates Model-View matrix.
	*/
	glm::mat4 getMV(const Camera2D& camera) const;
	glm::mat3 get_model_view3(const Camera2D& camera) const;
	
	glm::vec2* getPos() { return &pos_; }
	glm::vec2* getScale() { return &scale_; }

	void setPos(glm::vec2& pos) { this->pos_ = pos; }
	
	///*
	//\param ax axis of rotation
	//\param angleInRadians
	//*/
	//void rotate(const glm::vec3& ax, const float angleInRadians);

	///*
	//Add rotation using spinor.
	//\param spinor
	//*/
	//void rotate(const glm4::Spinor3D& spinor);

	/**
	Change scale values among each of axis.
	*/
	void setScale(glm::vec3& scale);

protected:
private:
	glm::vec2 pos_;
	float rot_;
	glm::vec2 scale_;
};

#endif