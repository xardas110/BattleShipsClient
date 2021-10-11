#pragma once
#include "BoundingShapes.h"
#include <DirectXCollision.h>

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
	static bool Intersect(const Rect* a, Poly* poly);
	static bool Intersect(const Point* a, Poly* poly);
	
	static bool Intersect(Poly* a, Poly* b);

	
};

