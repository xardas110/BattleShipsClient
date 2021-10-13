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
	CaseABIntersect(Segment, Segment);
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
	CaseBAIntersect(Segment, Segment);
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
	CaseBAIntersect(Segment, Segment);
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
		CaseBAIntersect(Segment, Segment);
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
std::shared_ptr<RPoint> closestPt;
void Collision::ClosestPoint(const glm::vec3& point, const Segment* segment, _Out_ float& t, _Out_ glm::vec3& d)
{
	const glm::vec3 ab = segment->B - segment->A;
	t = glm::dot(point - segment->A, ab) / glm::dot(ab, ab);
	t = glm::clamp(t, 0.f, 1.f);
	d = segment->A + t * ab;
	closestPt = RPoint::Create(d);
}

void Collision::ClosestPoint(const Point* point, const Segment* segment, float& t, glm::vec3& d)
{
	ClosestPoint(point->point, segment, t, d);
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

	//Move q back to world space
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
	const glm::vec3 segmentDistanceVector = segment->B - segment->A;
	const glm::vec3 d = glm::normalize(segmentDistanceVector);

	const glm::vec3 min = rect->GetMinBounds();
	const glm::vec3 max = rect->GetMaxBounds();

	tMin = 0.f;
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
	std::cout << "tmax: " << tMax << std::endl;
	q = p + d * tMin;
	return true;
}

bool Collision::Intersect(const Segment* segment, const Rect* rect)
{
	auto bMax = rect->GetMaxBounds();
	
	const glm::vec3 p0 = segment->A;
	const glm::vec3 p1 = segment->B;

	const glm::vec3 c = rect->C;
	const glm::vec3 e = bMax - c;

	glm::vec3 m = (p0 + p1) * 0.5f;

	glm::vec3 d = p1 - m;

	m = m - c;

	float adx = glm::abs(d.x);
	if (glm::abs(m.x) > e.x + adx) return false;
	float ady = glm::abs(d.y);
	if (glm::abs(m.y) > e.y + ady) return false;
	float adz = glm::abs(d.z);
	if (glm::abs(m.z) > e.z + adz) return false;
	
	adx += FLT_EPSILON;	ady += FLT_EPSILON;	adz += FLT_EPSILON;

	if (glm::abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return false;
	if (glm::abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return false;
	if (glm::abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return false;
	
	 return true;
}

float Collision::Signed2DTriArea(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	return (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);
}

bool Collision::Intersect(const Segment* segment, const Circle* circle)
{
	glm::vec3 d; float t;
	ClosestPoint(circle->C, segment, t, d);
	return glm::distance(d, circle->C) < circle->R;
}

bool Collision::Intersect(const Circle* circle, const Segment* segment)
{
	return Intersect(segment, circle);
}

bool Collision::Intersect(const Segment* segmentA, const Segment* segmentB, _Out_ float& t, _Out_ glm::vec3& p)
{
	const glm::vec3& a = segmentA->A;
	const glm::vec3& b = segmentA->B;
	const glm::vec3& c = segmentB->A;
	const glm::vec3& d = segmentB->B;

	// Sign of areas correspond to which side of ab points c and d are
	const float a1 = Signed2DTriArea(a, b, d); // Compute winding of abd (+ or -)
	const float a2 = Signed2DTriArea(a, b, c); // To intersect, must have sign opposite of a1

	// If c and d are on different sides of ab, areas have different signs
	if (a1 != 0.0f && a2 != 0.0f && a1 * a2 < 0.0f) {
		// Compute signs for a and b with respect to segment cd
		const float a3 = Signed2DTriArea(c, d, a); // Compute winding of cda (+ or -)
		// Since area is constant a1 - a2 = a3 - a4, or a4 = a3 + a2 - a1
		// float a4 = Signed2DTriArea(c, d, b); // Must have opposite sign of a3
		const float a4 = a3 + a2 - a1;
		// Points a and b on different sides of cd if areas have different signs
		if (a3 != 0.0f && a4 != 0.0f && a3 * a4 < 0.0f) {
			// Segments intersect. Find intersection point along L(t) = a + t * (b - a).
			// Given height h1 of an over cd and height h2 of b over cd,
			// t = h1 / (h1 - h2) = (b*h1/2) / (b*h1/2 - b*h2/2) = a3 / (a3 - a4),
			// where b (the base of the triangles cda and cdb, i.e., the length
			// of cd) cancels out.
			t = a3 / (a3 - a4);
			p = a + t * (b - a);
			return true;
		}
	}
	// Segments not intersecting (or collinear)
	return false;
}

bool Collision::Intersect(const Segment* segmentA, const Segment* segmentB)
{
	float t; glm::vec3 p;
	closestPt = RPoint::Create(p);
	if (Intersect(segmentA, segmentB, t, p))
	{
		std::cout << "lines intersecting " << std::endl;
		return true;
	}
	return false;
}

bool Collision::Intersect(const Segment* segment, const ORect* oRect)
{
	auto bMax = oRect->GetMaxBounds();

	const glm::vec3 p0 = segment->A;
	const glm::vec3 p1 = segment->B;

	const glm::vec3 c = oRect->C;
	const glm::vec3 e = bMax - c;

	glm::vec3 m = (p0 + p1) * 0.5f;

	glm::vec3 d = p1 - m;

	m = m - c;

	float adx = glm::abs(d.x);
	if (glm::abs(m.x) > e.x + adx) return false;
	float ady = glm::abs(d.y);
	if (glm::abs(m.y) > e.y + ady) return false;
	float adz = glm::abs(d.z);
	if (glm::abs(m.z) > e.z + adz) return false;

	adx += FLT_EPSILON;	ady += FLT_EPSILON;	adz += FLT_EPSILON;

	if (glm::abs(m.y * d.z - m.z * d.y) > e.y * adz + e.z * ady) return false;
	if (glm::abs(m.z * d.x - m.x * d.z) > e.x * adz + e.z * adx) return false;
	if (glm::abs(m.x * d.y - m.y * d.x) > e.x * ady + e.y * adx) return false;

	return true;
}

bool Collision::Intersect(const ORect* oRect, const Segment* segment)
{
	return Intersect(segment, oRect);
}

bool Collision::Intersect(const Rect* rect, const Segment* segment)
{
	return Intersect(segment, rect);
}

bool Collision::Intersect(const Segment* segment, const Point* point)
{
	glm::vec3 d; float t;
	ClosestPoint(point, segment, t, d);
	return glm::distance(d, point->point) < point->pointThickness;
}

bool Collision::Intersect(const Point* point, const Segment* segment)
{
	return Intersect(segment, point);
}

bool Collision::Intersect(const Segment* a, Poly* poly)
{
	switch (poly->GetType())
	{
		CaseBAIntersect(Rectangle, Rect);
		CaseBAIntersect(Circle, Circle);
		CaseBAIntersect(Point, Point);
		CaseBAIntersect(Segment, Segment);
	default: return false;
	}
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

