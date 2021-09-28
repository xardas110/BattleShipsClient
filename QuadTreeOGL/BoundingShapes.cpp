#include "BoundingShapes.h"

const glm::vec3& Rect::GetPosition() const
{
	return C;
}

const glm::vec3& Rect::GetScale() const
{
	return E;
}

glm::mat4 Rect::GetTranslation() const
{
	glm::mat4 model(1.f);

	model[0][0] = E.x;
	model[1][1] = E.y;

	model[3][0] = C.x;
	model[3][1] = C.y;

	return model;
}

glm::vec3 Rect::GetMinBounds() const
{
	return -E;
}

glm::vec3 Rect::GetMaxBounds() const
{
	return E;
}

void Rect::Print() const
{
	std::cout << "Centre: " << C.x << " " << C.y << " Extents: " << E.x << " " << E.y << std::endl;
}

bool Rect::Intersect(const Point& point) const
{
	const auto pointC = point.point - this->C;
	return pointC.x < this->E.x&& pointC.x > -this->E.x && pointC.y < this->E.y&& pointC.y > -this->E.y;
}

glm::mat4 Point::GetTranslation() const
{
	glm::mat4 model(1.f);
	model[3][0] = point.x;
	model[3][1] = point.y;
	return model;
}

bool Point::Intersect(const Rect& rect) const
{
	const auto pointC = point - rect.C;
	return pointC.x < rect.E.x&& pointC.x > -rect.E.x && pointC.y < rect.E.y&& pointC.y > -rect.E.y;
}
