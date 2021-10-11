#pragma once
#include "../include/glm/glm.hpp"
#include "../include/glm/gtc/matrix_transform.hpp"


class Camera
{
private:
	glm::vec3		translate;
	glm::mat4		view, projection;
	glm::vec3		velocity;
public:
	Camera();
	void			SetLookAt(glm::vec3 pos, glm::vec3 dir, glm::vec3 up);
	void			SetProjection(float fov, float aspect, float near, float far);
	void			SetTranslate(glm::vec3 trans);
	void			AddVelocityX(const float x);
	void			AddVelocityY(const float y);
	void			AddVelocityZ(const float z);
	void			UpdatePosition(const double &deltaTime);

	const glm::vec3 GetPosition() const;
	const glm::mat4 GetViewMatrix() const;
	const glm::mat4 GetProjectionMatrix() const;
};

