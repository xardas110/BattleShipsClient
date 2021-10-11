#pragma once
#include "../include/glm/glm.hpp"
#include "../Engine/BoundingShapes.h"

class RayCast
{
public:
	
	RayCast(glm::vec3 P, glm::vec3 D)
		:P{ P }, D{ D }
	{};

	bool Intersect(const Rect& rect, float& tmin, glm::vec3& q);


	glm::vec3 P, D;
};

