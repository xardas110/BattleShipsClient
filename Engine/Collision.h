#pragma once
#include "BoundingShapes.h"

//Only using SIMD for OBB-OBB intersection
//TODO: Segment-OOB and Segment - segment to finish segment intersections
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
	static void ClosestPoint(const glm::vec3& point, const Segment* segment, _Out_ float& t, _Out_ glm::vec3& d);
	static void ClosestPoint(const Point* point, const Segment* segment, _Out_ float& t, _Out_ glm::vec3& d);
	
	static bool Intersect(const ORect* oRect, const Circle* circle);
	static bool Intersect(const Circle* circle, const ORect* oRect);

	static bool Intersect(const ORect* oRect, const Point* point);
	static bool Intersect(const Point* point, const ORect* oRect);

	//*Works for rays too*//
	static bool Intersect(const Segment* segment, const Rect* rect, _Out_ float &tMin, _Out_ glm::vec3 &q);

	static bool Intersect(const Segment* segment, const Rect* rect);

	static float Signed2DTriArea(const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

	/*Works only for 2D*/ //TODO: FIX this NOT WORKING
	static bool Intersect(const Segment* segmentA, const Segment* segmentB, _Out_ float &t, _Out_ glm::vec3 &p);
	/*Works only for 2D*/ //TODO: FIX this NOT WORKING
	static bool Intersect(const Segment* segmentA, const Segment* segmentB);

	static bool Intersect(const Segment* segment, const ORect* oRect);
	
	static bool Intersect(const ORect* oRect, const Segment* segment);
	
	static bool Intersect(const Rect* rect, const Segment* segment);
	
	static bool Intersect(const Rect* a, Poly* poly);
	
	static bool Intersect(const Segment* segment, const Circle* circle);
	static bool Intersect(const Circle* circle, const Segment* segment);


	static bool Intersect(const Segment* segment, const Point* point);
	static bool Intersect(const Point* point, const Segment* segment);
	static bool Intersect(const Segment* a, Poly* poly);

	static bool Intersect(Poly* a, Poly* b);

};

