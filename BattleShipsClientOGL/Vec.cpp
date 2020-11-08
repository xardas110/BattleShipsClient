#include "Vec.h"

iVec2D::iVec2D(int x, int y)
	:x(x), y(y)
{
}

iVec2D::iVec2D()
	: x{ 0 }, y{0}
{
}

iVec2D iVec2D::operator+(iVec2D cpy)
{
	iVec2D result;
	result.x = x + cpy.x;
	result.y = y + cpy.y;
	return result;
}

iVec2D iVec2D::operator-(iVec2D cpy)
{
	iVec2D result;
	result.x = x - cpy.x;
	result.y = y - cpy.y;
	return result;
}

iVec2D iVec2D::operator*(iVec2D cpy)
{
	iVec2D result;
	result.x = x * cpy.x;
	result.y = y * cpy.y;
	return result;
}

iVec2D iVec2D::operator=(iVec2D cpy)
{
	x = cpy.x;
	y = cpy.y;
	return *this;
}

iVec2D iVec2D::operator/(int val)
{
	iVec2D result;
	result.x = x / val;
	result.y = y / val;
	return result;
}
