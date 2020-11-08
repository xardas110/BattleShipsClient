#include "Camera.h"

Camera::Camera()
	:velocity({0.f, 0.f, 0.f}) //:fov(45.f), aspect(0.f), near(0.1f), far(100.f)
{
}

void Camera::SetLookAt(glm::vec3 pos, glm::vec3 dir, glm::vec3 up)
{
	view = glm::lookAt(pos, dir, up);
	translate = pos;
}

void Camera::SetProjection(float fov, float aspect, float near, float far)
{
	//this->fov = fov;
	//this->near = near;
	//this->far = far;
	//this->aspect = aspect;
	projection = glm::perspective(glm::radians(fov), aspect, near, far);
}

void Camera::SetTranslate(glm::vec3 trans)
{
	translate += trans;
	view = glm::translate(view, -trans);
}

const glm::vec3 Camera::GetPosition() const
{
	return translate;
}

void Camera::AddVelocityX(const float x)
{
	velocity.x = x;
}

void Camera::AddVelocityY(const float y)
{
	velocity.y = y;
}

void Camera::AddVelocityZ(const float z)
{
	velocity.z = z;
}

void Camera::UpdatePosition(const double& deltaTime)
{
	SetTranslate(velocity * (float)deltaTime);
}

const glm::mat4 Camera::GetViewMatrix() const
{
	return view;
}

const glm::mat4 Camera::GetProjectionMatrix() const
{
	return projection;
}
