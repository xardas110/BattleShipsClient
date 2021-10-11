#include "RayCast.h"
#include <iostream>

bool RayCast::Intersect(const Rect& rect, float &tmin, glm::vec3 &q)
{
	tmin = 0.f;
	float tmax = FLT_MAX;

	auto min = rect.GetMinBounds();
	auto max = rect.GetMaxBounds();

	const auto pc = rect.C;
	
	for (auto i= 0; i< 3; i++)
		if (D[i] < FLT_EPSILON)
		{
			std::cout << "running if statment " << std::endl;
			if (pc[i] < min[i] || pc[i] > max[i]) return false;
		}
		else
		{
			float ood = 1.f / D[i];
			float t1 = (min[i] - pc[i]) * ood;
			float t2 = (max[i] - pc[i]) * ood;

			if (t1 > t2) std::swap(t1, t2);
			if (t1 > tmin) tmin = t1;
			if (t2 > tmax) tmax = t2;
			if (tmin > tmax) return false;
		}

	q = P + D * tmin;
	return true;
}
