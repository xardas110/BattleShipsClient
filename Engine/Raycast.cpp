#include "Raycast.h"
#include "Debug.h"
#include <iostream>
RayCast::RayCast(_In_ const glm::vec3 orig, _In_ const glm::vec3 dir)
	:ORIG(_mm_set_ps(0.f, orig.z, orig.y, orig.x)), DIR(_mm_set_ps(0.f, dir.z, dir.y, dir.x))
{
	INVDIR = _mm_div_ps(_mm_set_ps(1.f, 1.f, 1.f, 1.f), DIR);
	ORIG.m128_f32[3] = DIR.m128_f32[0] < 0.f;
	DIR.m128_f32[3] = DIR.m128_f32[1] < 0.f;
	INVDIR.m128_f32[3] = DIR.m128_f32[2] < 0.f;
	//std::cout << ORIG.m128_f32[3] << " " << DIR.m128_f32[3] << " " << INVDIR.m128_f32[3] << std::endl;
}

bool RayCast::Intersect(_In_ const Bounding::Box& b)
{
	float  txMin, txMax, tyMin, tyMax, tzMin, tzMax;
	txMin = (b.Bounds[(int)(this->ORIG.m128_f32[3])].m128_f32[0] - this->ORIG.m128_f32[0]) * this->INVDIR.m128_f32[0];
	txMax = (b.Bounds[1 - (int)this->ORIG.m128_f32[3]].m128_f32[0] - this->ORIG.m128_f32[0]) * this->INVDIR.m128_f32[0];

	tyMin = (b.Bounds[(int)this->DIR.m128_f32[3]].m128_f32[1] - this->ORIG.m128_f32[1]) * this->INVDIR.m128_f32[1];
	tyMax = (b.Bounds[1 - (int)this->DIR.m128_f32[3]].m128_f32[1] - this->ORIG.m128_f32[1]) * this->INVDIR.m128_f32[1];

	

	if ((txMin > tyMax) || (tyMin > txMax))
		return false;
	if (tyMin > txMin)
		txMin = tyMin;
	if (tyMax < txMax)
		txMax = tyMax;

	tzMin = (b.Bounds[(int)this->INVDIR.m128_f32[3]].m128_f32[2] - this->ORIG.m128_f32[2]) * this->INVDIR.m128_f32[2];
	tzMax = (b.Bounds[1 - (int)this->INVDIR.m128_f32[3]].m128_f32[2] - this->ORIG.m128_f32[2]) * this->INVDIR.m128_f32[2];

	if ((txMin > tzMax) || (tzMin > txMax))
		return false;
	if (tzMin > txMin)
		txMin = tzMin;
	if (tzMax < txMax)
		txMax = tzMax;

	return true;
}

bool RayCast::Intersect(_In_ const Bounding::Box& b, float &tMin)
{
	float  txMin, txMax, tyMin, tyMax, tzMin, tzMax;
	txMin = (b.Bounds[(int)(this->ORIG.m128_f32[3])].m128_f32[0] - this->ORIG.m128_f32[0]) * this->INVDIR.m128_f32[0];
	txMax = (b.Bounds[1 - (int)this->ORIG.m128_f32[3]].m128_f32[0] - this->ORIG.m128_f32[0]) * this->INVDIR.m128_f32[0];

	tyMin = (b.Bounds[(int)this->DIR.m128_f32[3]].m128_f32[1] - this->ORIG.m128_f32[1]) * this->INVDIR.m128_f32[1];
	tyMax = (b.Bounds[1 - (int)this->DIR.m128_f32[3]].m128_f32[1] - this->ORIG.m128_f32[1]) * this->INVDIR.m128_f32[1];

	std::cout << "Failed 0 check" << std::endl;
	
	if ((txMin > tyMax) || (tyMin > txMax))
		return false;
	if (tyMin > txMin)
		txMin = tyMin;
	if (tyMax < txMax)
		txMax = tyMax;

	std::cout << "Failed 1 check" << std::endl;
	
	tzMin = (b.Bounds[(int)this->INVDIR.m128_f32[3]].m128_f32[2] - this->ORIG.m128_f32[2]) * this->INVDIR.m128_f32[2];
	tzMax = (b.Bounds[1 - (int)this->INVDIR.m128_f32[3]].m128_f32[2] - this->ORIG.m128_f32[2]) * this->INVDIR.m128_f32[2];

	if ((txMin > tzMax) || (tzMin > txMax))
		return false;
	if (tzMin > txMin)
		txMin = tzMin;
	if (tzMax < txMax)
		txMax = tzMax;
	std::cout << "Failed 2 check" << std::endl;
	//Dont have to take all the t's, because the ray is always rotated in the direction where it gives tmin as the lowest value
	tMin = fmaxf(fmaxf(txMin, tyMin), tzMin);
	return true;
}