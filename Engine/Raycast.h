#pragma once
#include "../include/glm/glm.hpp"
#include "BoundingShapes.h"
#include <xmmintrin.h>

//**SIGN is in orig.w, dir.w, invdir.w as xyz starting from former to latter **//
class RayCast
{
private:
	//for future reasons I will use vec4 for all math operations here because of SIMD instructions, even tho current code probably isn't that optimized
	//**SIGN.xyz is in subsequent order in orig.w, dir.w, invdir.w **//
	__m128 ORIG, DIR, INVDIR;
public:
	RayCast(_In_ const glm::vec3 orig, _In_ const glm::vec3 dir);
	bool _vectorcall Intersect(_In_ const Box& b);
	bool _vectorcall Intersect(_In_ const Box& b, float& tMin);
};
