#pragma once
#include "../include/glm/glm.hpp"
inline void PrintMatrix(glm::mat4 m)
{
	printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n\n\n",
		m[0].x, m[0].y, m[0].z, m[0].w,
		m[1].x, m[1].y, m[1].z, m[1].w,
		m[2].x, m[2].y, m[2].z, m[2].w,
		m[3].x, m[3].y, m[3].z, m[3].w
	);
}
inline void PrintVector(glm::vec3 v)
{
	printf("%f %f %f \n\n\n",
		v.x, v.y, v.z

	);
}

inline void PrintVector(__m128 v)
{
	printf("%f %f %f %f\n\n\n",
		v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]

	);
}