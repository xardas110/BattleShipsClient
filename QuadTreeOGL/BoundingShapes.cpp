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
	return C-E;
}

glm::vec3 Rect::GetMaxBounds() const
{
	return C+E;
}

void Rect::Print() const
{
	std::cout << "Centre: " << C.x << " " << C.y << " Extents: " << E.x << " " << E.y << std::endl;
}

bool Rect::Intersect(::Poly* poly)
{
	switch (poly->GetType())
	{
	case ::Poly::Types::Null:
		break;
	case ::Poly::Types::Point:
		return IntersectPoint(dynamic_cast<::Point*>(poly));
	case ::Poly::Types::Rectangle:
		return IntersectRect(dynamic_cast<::Rect*>(poly));
	case ::Poly::Types::Circle:
		return IntersectCircle(dynamic_cast<::Circle*>(poly));
	default:
		throw std::exception("Lack of intersection functions");
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

bool Rect::IntersectCircle(const ::Circle* circle) const
{
	//compute closest point in box to point
	const auto PC = circle->C;
	const auto BMin = this->C - this->E;
	const auto BMax = this->C + this->E;

	::Point closestInBox{};
	for (auto i = 0; i < 3; i++)
	{
		if (PC[i] > BMin[i] && PC[i] < BMax[i])
			closestInBox.point[i] = PC[i];
		else
		{
			if (PC[i] < BMin[i])
				closestInBox.point[i] = BMin[i];
			else
				closestInBox.point[i] = BMax[i];
		}
	}

	return glm::distance(closestInBox.point, circle->C) < circle->R;
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
	case ::Poly::Types::Null:
		break;
	case ::Poly::Types::Rectangle:
		return IntersectRect(dynamic_cast<::Rect*>(poly));
	case ::Poly::Types::Point:
		return IntersectPoint(dynamic_cast<::Point*>(poly));
	case ::Poly::Types::Circle:
		return IntersectCircle(dynamic_cast<::Circle*>(poly));
	default:
		throw std::exception("Lack of intersection function");
	}
	return false;
}

bool Point::IntersectRect(const Rect* rect) const
{
	const auto pointC = point - rect->C;
	return pointC.x < rect->E.x&& pointC.x > -rect->E.x && pointC.y < rect->E.y&& pointC.y > -rect->E.y;
}

bool Point::IntersectPoint(const Point* point) const
{
	if (glm::distance(point->point, this->point) < point->pointThickness + this->pointThickness)
		return true;
	return false;
}

bool Point::IntersectCircle(const ::Circle* circle) const
{
	return glm::distance(this->point, circle->C) < circle->R + this->pointThickness;
}

Circle::Circle(glm::vec3 centre, float radius)
	:Poly(Types::Circle), C(centre), R(radius)
{
	
}

glm::mat4 Circle::GetTranslation() const
{
	glm::mat4 model(1.f);
	model[0][0] = R;
	model[1][1] = R;
	model[2][2] = R;
	model[3][0] = C.x;
	model[3][1] = C.y;
	model[3][2] = C.z;
	return model;
}

bool Circle::Intersect(Poly* poly)
{
	switch (poly->GetType())
	{
		case Poly::Types::Null:
			break;
		case Poly::Types::Point:
			return IntersectPoint(dynamic_cast<::Point*>(poly));
		case Poly::Types::Circle:
			return IntersectCircle(dynamic_cast<::Circle*>(poly));
		case Poly::Types::Rectangle:
			return IntersectRect(dynamic_cast<::Rect*>(poly));
		default:
			throw std::exception("lacking intersection method");
	}
	return false;
}

bool Circle::IntersectPoint(const ::Point* point) const
{
	return glm::distance(point->point, this->C) < this->R + point->pointThickness;
}

bool Circle::IntersectRect(const ::Rect* rect) const
{
	//compute closest point in box to point
	const auto PC = this->C;
	const auto BMin = rect->C - rect->E;
	const auto BMax = rect->C + rect->E;

	::Point closestInBox{};
	for (auto i = 0; i < 3; i++)
	{
		if (PC[i] > BMin[i] && PC[i] < BMax[i])
			closestInBox.point[i] = PC[i];
		else
		{
			if (PC[i] < BMin[i])
				closestInBox.point[i] = BMin[i];
			else
				closestInBox.point[i] = BMax[i];
		}
	}

	return glm::distance(closestInBox.point, this->C) < this->R;
}

bool Circle::IntersectCircle(const ::Circle* circle) const
{
	return glm::distance(circle->C, this->C) < this->R + circle->R;
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
