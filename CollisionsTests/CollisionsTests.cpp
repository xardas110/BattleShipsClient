#include "../QuadTreeOGL/BoundingShapes.h"

#include <iostream>

int main()
{
	/*
	// AABB to AABB
    Rect A({ 0.f, 0.f, 0.f }, { 4.f, 4.f, 0.f });
    Rect B({ -8.0f, -8.1f, 0.f }, { 4.f, 4.f, 0.f });
    auto* bPtr = &B;

   if ( A.Intersect(bPtr))
   { 
       std::cout << "intersecting" << std::endl;
   }
   else
   {
       std::cout << "nope" << std::endl;
   }
   */

	//AABB to circle
    Rect A({ 0.f, 0.f, 0.f }, { 4.f, 4.f, 0.f });
    Circle B({ -4.71, 4.71f, 0.f },  1.f );
	
    auto* aPtr = &A;
    if (B.IntersectRect(aPtr))
    {
        std::cout << "intersecting" << std::endl;
    }
    else
    {
        std::cout << "nope" << std::endl;
    }
}
