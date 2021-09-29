#include "../QuadTreeOGL/BoundingShapes.h"

#include <iostream>

int main()
{
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
}
