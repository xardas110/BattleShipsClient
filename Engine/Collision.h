#pragma once
#include "BoundingShapes.h"

//Only using SIMD for OBB-OBB intersection
struct Collision
{
	static bool Intersect(const Rect* rect, const Circle* circle);
	static bool Intersect(const Rect* rect, const Rect* rect1);
	static bool Intersect(const Rect* rect, const Point* point);
		
	static bool Intersect(const Point* point, const Rect* rect);
	static bool Intersect(const Circle* circle, const Rect* rect);
	static bool Intersect(const Circle* circle, const Circle* circle1);
	static bool Intersect(const Circle* circle, const Point* point);
	static bool Intersect(const Point* point, const Circle* rect);
	static bool Intersect(const Point* point, const Point* point1);

	static bool Intersect(const Circle* a, Poly* poly);
	static bool Intersect(const Point* a, Poly* poly);
	
	static bool Intersect(const ORect* a, Poly* poly);
	static bool Intersect(const ORect* oRectA, const ORect* oRectB);
	static bool Intersect(const Rect* rect, const ORect* oRect);
	static bool Intersect(const ORect* oRect, const Rect* rect);

	static void ClosestPoint(const ORect* oRect, const glm::vec3& p, glm::vec3& _Out_ q);
	static void ClosestPoint(const Rect* rect, const glm::vec3& p, glm::vec3& _Out_ q);
	static void ClosestPoint(const Point* point, const Segment* segment, _Out_ float& t, _Out_ glm::vec3& d);
	
	static bool Intersect(const ORect* oRect, const Circle* circle);
	static bool Intersect(const Circle* circle, const ORect* oRect);

	static bool Intersect(const ORect* oRect, const Point* point);
	static bool Intersect(const Point* point, const ORect* oRect);

	//*Works for rays too*//
	static bool Intersect(const Segment* segment, const Rect* rect, _Out_ float &tMin, _Out_ glm::vec3 &q);

	static bool Intersect(const Segment* segment, const Rect* rect);
	
	
	static bool Intersect(const Rect* rect, const Segment* segment);
	
	static bool Intersect(const Rect* a, Poly* poly);
	static bool Intersect(const Segment* a, Poly* poly);

	static bool Intersect(Poly* a, Poly* b);

};

