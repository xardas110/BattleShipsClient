#pragma once

class iVec2D
{
public:
	iVec2D(int x, int y);
	iVec2D();
	int x, y;
	iVec2D operator+(iVec2D cpy);
	iVec2D operator-(iVec2D cpy);
	iVec2D operator*(iVec2D cpy);
	iVec2D operator=(iVec2D cpy);
	iVec2D operator/(int val);
};

