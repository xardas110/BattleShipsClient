#pragma once
#include <iostream>
#include "../include/glm/glm.hpp"
#include "../include/glm/gtx/transform.hpp"
#include <vector>
#include "BoundingShapes.h"

#define MAX_POINTS_PR_QUAD 4



class QuadTree
{
	enum Coord
	{
		NW, NE, SE, SW, Size
	};

public:
	QuadTree(const Rect& rect);
	
	bool IsSubDivided() const;

	void SubDivide();
	void SubDivide(const int n);

	void Insert(const Point &point);
	
	void GetAllQuads(std::vector<Rect>& container) const;
	void GetAllPoints(std::vector<Point>& container) const;
	
	void PrintAllQuads() const;
	
	const Rect& GetBounds() const;
private:
	Rect bounds;
	std::vector<Point> points;
	QuadTree** Nodes;
};

