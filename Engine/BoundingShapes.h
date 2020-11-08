#pragma once
#include "../include/glm/gtc/matrix_transform.hpp"
#include "../include/glm/glm.hpp"
#include <xmmintrin.h>

namespace Bounding
{ 
//Gotta optimze this data since much of it will run inside loops
	//might aswell do 3d intersections instead of 2d cuz of SIMD instructions
	_declspec(align(16)) struct Box
	{
		Box()
			:Center{ 0.f, 0.f, 0.f, 0.f }, Extents{ 0.5f, 0.5f, 0.0f, 0.f }
		{
			UpdateBounds();
		}
		Box(glm::vec3 center, glm::vec3 extents)
			:Center({ center.x, center.y, center.z, 0.f }), Extents({extents.x, extents.y, extents.z, 0.f})
		{
			UpdateBounds();
		}
		__m128				Center;
		__m128				Extents;
		__m128				Bounds[2U];

		void _vectorcall	SetCenter(_In_ glm::vec3 pos);
		void _vectorcall	SetExtents(_In_ glm::vec3 pos);
		void _vectorcall	UpdateBounds();
	};
}