#include "BoundingShapes.h"

void _vectorcall Bounding::Box::SetCenter(glm::vec3 pos)
{
	Center = _mm_set_ps(0.f, pos.z, pos.y, pos.x);
	UpdateBounds();
}

void _vectorcall Bounding::Box::SetExtents(glm::vec3 scale)
{
	Extents = _mm_set_ps(0.f, scale.z * 0.5f, scale.y * 0.5f, scale.x * 0.5f);
	UpdateBounds();
}

void Bounding::Box::UpdateBounds()
{
	Bounds[0] = _mm_sub_ps(Center, Extents);
	Bounds[1] = _mm_add_ps(Center, Extents);
}