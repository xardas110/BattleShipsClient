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

bool Rect::Intersect(::Poly* poly)
{
	switch (poly->GetType())
	{
	case ::Poly::Types::Point:
		return IntersectPoint(dynamic_cast<::Point*>(poly));
	case ::Poly::Types::Rectangle:
		return IntersectRect(dynamic_cast<::Rect*>(poly));
	default:
		break;
	}
	return false;
}

bool Rect::IntersectPoint(const ::Point* point) const
{
	std::cout << "intersect point running " << std::endl;
	const auto pointC = point->point - this->C;
	return pointC.x < this->E.x&& pointC.x > -this->E.x && pointC.y < this->E.y&& pointC.y > -this->E.y;
}

bool Rect::IntersectRect(const::Rect* rect) const
{
	const auto BC = rect->C - this->C;
	const auto BMin = BC - rect->E;
	const auto BMax = BC + rect->E;
	const auto AMin = -this->E;
	const auto AMax = this->E;

	for (auto i = 0; i < 3; i++)
	{
		
		if (BMax[i] < AMin[i] || BMin[i] > AMax[i])
		{
			std::cout << "intersect rect FAILED" << std::endl;
			return false;
		}
	}
	std::cout << "intersect rect returns true" << std::endl;
	return true;
}

glm::mat4 Point::GetTranslation() const
{
	glm::mat4 model(1.f);
	model[3][0] = point.x;
	model[3][1] = point.y;
	return model;
}

bool Point::Intersect(::Poly* poly)
{
	switch (poly->GetType())
	{
	case ::Poly::Types::Rectangle:
		return IntersectRect(dynamic_cast<::Rect*>(poly));
	default:
		break;
	}
	return false;
}

bool Point::IntersectRect(const Rect* rect) const
{
	const auto pointC = point - rect->C;
	return pointC.x < rect->E.x&& pointC.x > -rect->E.x && pointC.y < rect->E.y&& pointC.y > -rect->E.y;
}


Poly::Poly(Types type)
	: Type(type)
{
}

bool Poly::Intersect(Poly* poly)
{
	//do nothing
	return false;
}

::Poly::Types Poly::GetType() const
{
	return ::Poly::Type;
}
