#include "Collision.h"

#include "../include/glm/geometric.hpp"

#define CaseABIntersect(enum, type) \
	case Poly::enum: return Intersect(dynamic_cast<type*>(a), b)

#define CaseBAIntersect(enum, type) \
	case Poly::enum: return Intersect(a, dynamic_cast<type*>(poly))



bool Collision::Intersect(Poly* a, Poly* b)
{
	switch (a->GetType())
	{
	CaseABIntersect(Rectangle, Rect);
	CaseABIntersect(Circle, Circle);
	CaseABIntersect(Point, Point);
	default: return false;
	}
}

bool Collision::Intersect(const Circle* a, Poly* poly)
{
	switch (poly->GetType())
	{
	CaseBAIntersect(Rectangle, Rect);
	CaseBAIntersect(Circle, Circle);
	CaseBAIntersect(Point, Point);
	default: return false;
	}
}

bool Collision::Intersect(const Rect* a, Poly* poly)
{
	switch (poly->GetType())
	{
	CaseBAIntersect(Rectangle, Rect);
	CaseBAIntersect(Circle, Circle);
	CaseBAIntersect(Point, Point);
	default: return false;
	}
}


bool Collision::Intersect(const Point* a, Poly* poly)
{

	switch (poly->GetType())
	{
		CaseBAIntersect(Rectangle, Rect);
		CaseBAIntersect(Circle, Circle);
		CaseBAIntersect(Point, Point);
		default: return false;
	}
}

bool Collision::Intersect(const Point* point, const Point* point1)
{
	return glm::distance(point->point, point1->point) < point->pointThickness + point1->pointThickness;
}

bool Collision::Intersect(const Rect* rect, const Circle* circle)
{
	//compute closest point in box to point
	const auto PC = circle->C;
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

	return glm::distance(closestInBox.point, circle->C) < circle->R;
}

bool Collision::Intersect(const Rect* rect, const Rect* rect1)
{
	const auto BC = rect->C - rect1->C;
	const auto BMin = BC - rect->E;
	const auto BMax = BC + rect->E;
	const auto AMin = -rect1->E;
	const auto AMax = rect1->E;

	for (auto i = 0; i < 3; i++)
		if (BMax[i] < AMin[i] || BMin[i] > AMax[i])
			return false;

	return true;
}

bool Collision::Intersect(const Rect* rect, const Point* point)
{
	const auto pointC = point->point - rect->C;
	return pointC.x < rect->E.x&& pointC.x > -rect->E.x && pointC.y < rect->E.y&& pointC.y > -rect->E.y;
}

bool Collision::Intersect(const Circle* circle, const Circle* circle1)
{
	return glm::distance(circle->C, circle1->C) < circle1->R + circle->R;
}

bool Collision::Intersect(const Circle* circle, const Point* point)
{
	return glm::distance(point->point, circle->C) < circle->R + point->pointThickness;
}

bool Collision::Intersect(const Point* point, const Rect* rect)
{
	return Intersect(rect, point);
}

bool Collision::Intersect(const Point* point, const Circle* rect)
{
	return Intersect(rect, point);
}

bool Collision::Intersect(const Circle* circle, const Rect* rect)
{
	return Intersect(rect, circle);
}

