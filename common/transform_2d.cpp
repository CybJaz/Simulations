#include "transform_2d.h"

#include <glm/glm.hpp>
//#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/transform.hpp>

Transform2D::Transform2D(const glm::vec2& pos, const float& angle, const glm::vec2& scale)
	: pos_(pos), rot_(angle), scale_(scale)
{}

//Transform2D Transform2D::rotation(float& rotation)
//{
//	return Transform2D(glm::vec3(), rotation);
//}
//
//Transform2D Transform2D::scale(glm::vec3& scale)
//{
//	return Transform2D(glm::vec3(), glm::vec3(), scale);
//}
//
//Transform2D Transform2D::translation(glm::vec3& translation)
//{
//	return Transform2D(translation);
//}
//
//glm::vec3 Transform2D::operator () (const glm::vec3& point) const
//{
//	return glm::vec3(getModel() * glm::vec4(point, 1.0f));
//}
//
//glm::vec3 Transform2D::rotatePoint(const glm::vec3& point) const
//{
//	return spinor_.toMat3() * point;
//}
//
//glm::vec3 Transform2D::transformPoint(const glm::vec3& point) const
//{
//	return glm::vec3(getModel() * glm::vec4(point, 0.0f));
//}

glm::mat4 Transform2D::getModel() const
{
	//glm::mat4 posMat = glm::translate(pos_);
	//glm::mat4 scaleMat = glm::scale(scale_);

	//return posMat * spinor_.toMat4() * scaleMat;

	return glm::mat4();
}

////glm::mat4 Transform2D::getRotationMat4() const
////{
////	return spinor_.toMat4();
////}
////
////glm::mat3 Transform2D::getRotationMat3() const
////{
////	return spinor_.toMat3();
////}

glm::mat4 Transform2D::getMVP(const Camera2D& camera) const
{
	glm::mat4 VP = camera.get_view_projection();
	glm::mat4 M = getModel();

	return VP * M;//camera.GetViewProjection() * GetModel();
}

glm::mat3 Transform2D::getMVP3(const Camera2D& camera) const
{
	glm::mat3 VP = camera.get_view_projection3();
	//glm::mat4 M = getModel();

	return VP;//camera.GetViewProjection() * GetModel();
}

glm::mat3 Transform2D::get_model_view3(const Camera2D& camera) const
{
	return camera.get_view3();
}

//glm::mat4 Transform2D::getMV(const Camera& camera) const
//{
//	glm::mat4 V = camera.getView();
//	glm::mat4 M = getModel();
//
//	return V * M;//camera.GetViewProjection() * GetModel();
//}
//
//void Transform2D::rotate(const glm::vec3& ax, const float ang)
//{
//	spinor_ *= glm4::Spinor3D(ang, glm4::bivec3(ax));
//};
//
//void Transform2D::rotate(const glm4::Spinor3D& spinor)
//{
//	spinor_ *= spinor;
//};
//
//void Transform2D::setScale(glm::vec3& scale) { this->scale_ = scale; }