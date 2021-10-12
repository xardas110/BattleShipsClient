#include "Collision.h"

#include <iostream>

#include "Debug.h"
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
	CaseABIntersect(ORectangle, ORect);
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
	CaseBAIntersect(ORectangle, ORect);
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
	CaseBAIntersect(ORectangle, ORect);
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
		CaseBAIntersect(ORectangle, ORect);
		default: return false;
	}
}

bool Collision::Intersect(const ORect* a, Poly* poly)
{
	switch (poly->GetType())
	{
		CaseBAIntersect(Rectangle, Rect);
		CaseBAIntersect(ORectangle, ORect);
		CaseBAIntersect(Circle, Circle);
		CaseBAIntersect(Point, Point);
	default: return false;
	}
}

bool Collision::Intersect(const ORect* oRectA, const ORect* oRectB)
{
	OrientedBox OBA(oRectA->C, oRectA->E, oRectA->orient);
	const OrientedBox OBB(oRectB->C, oRectB->E, oRectB->orient);

	return OBA.Intersect(OBB);
}

bool Collision::Intersect(const Point* point, const Point* point1)
{
	return glm::distance(point->point, point1->point) < point->pointThickness + point1->pointThickness;
}

bool Collision::Intersect(const Rect* rect, const Circle* circle)
{
	//compute closest point in box to point
	glm::vec3 closestInBox{};
	ClosestPoint(rect, circle->C, closestInBox);
	
	return glm::distance(closestInBox, circle->C) < circle->R;
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

bool Collision::Intersect(const Rect* rect, const ORect* oRect)
{
	//Make both oriented boxes
	OrientedBox OBA(oRect->C, oRect->E, oRect->orient);
	OrientedBox OBB(rect->C, rect->E, glm::identity<glm::quat>());
	
	return OBA.Intersect(OBB);
}

bool Collision::Intersect(const ORect* oRect, const Rect* rect)
{
	return Intersect(rect, oRect);
}

void Collision::ClosestPoint(const ORect* oRect, const glm::vec3& p, glm::vec3& _Out_ q)
{
	const glm::vec3 e = oRect->E;

	//Get axises from the matrix
	const glm::mat4 rm = glm::toMat4(oRect->orient);
	
	//Normalize axises
	glm::vec3 u[3]{};
	for (auto i = 0; i< 3; i++)
		u[i] = glm::normalize(glm::vec3(rm[i]));

	// Transfer point to box coordinate system
	const glm::vec3 d = p - oRect->C;

	q = oRect->C;

	for (int i= 0; i< 3; i++)
	{
		float dist = glm::dot(d, u[i]);

		dist = glm::clamp(dist, -e[i], e[i]);
		
		q += dist * u[i];
	}	
}

void Collision::ClosestPoint(const Rect* rect, const glm::vec3& p, glm::vec3& q)
{
	//compute closest point in box to point
	const auto d = p - rect->C;
	const auto e = rect->E;

	for (int i = 0; i < 3; i++)
		q[i] = glm::clamp(d[i], -e[i], e[i]);

	//Move q back the world space
	q += rect->C;
}

bool Collision::Intersect(const ORect* oRect, const Circle* circle)
{
	glm::vec3 closestPoint{};
	ClosestPoint(oRect, circle->C, closestPoint);

	return glm::distance(closestPoint, circle->C) < circle->R;
}

bool Collision::Intersect(const Circle* circle, const ORect* oRect)
{
	return Intersect(oRect, circle);
}

bool Collision::Intersect(const ORect* oRect, const Point* point)
{
	//Recreate point as a circle
	Circle c(point->point, point->pointThickness);
	return Intersect(oRect, &c);
}

bool Collision::Intersect(const Point* point, const ORect* oRect)
{
	return Intersect(oRect, point);
}

bool Collision::Intersect(const Segment* segment, const Rect* rect, _Out_ float& tMin, _Out_ glm::vec3& q)
{
	const glm::vec3 p = segment->A;
	const glm::vec3 segmentDistanceVector = segment->B - p;
	const glm::vec3 d = glm::normalize(segmentDistanceVector);

	const glm::vec3 min = rect->GetMinBounds();
	const glm::vec3 max = rect->GetMaxBounds();

	tMin = -FLT_MAX;
	float tMax = glm::length(segmentDistanceVector);
	
	for (int i = 0; i<3; i++)
	{
		if (abs(d[i]) < FLT_EPSILON)
		{
			//Parallel to slab
			if (p[i] < min[i] || p[i] > max[i]) return false;
		}
		else
		{
			const float ood = 1.f / d[i];
			float t1 = (min[i] - p[i]) * ood;
			float t2 = (max[i] - p[i]) * ood;

			if (t1 > t2) std::swap(t1, t2);

			if (t1 > tMin) tMin = t1;
			if (t2 > tMax) tMax = t2;
			
			if (tMin > tMax) return false;
		}
			
	}
	q = p + d * tMin;
	return true;
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

