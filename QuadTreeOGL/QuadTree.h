#pragma once
#include <vector>
#include "../Engine/BoundingShapes.h"

#define MAX_POINTS_PR_QUAD 1

class QuadTree
{
	enum Coord
	{
		NW, NE, SE, SW, Size
	};

public:
	QuadTree(const std::shared_ptr<RBRect>& rect);
	
	bool IsSubDivided() const;

	void SubDivide();
	void SubDivide(const int n);

	bool Insert(std::shared_ptr<Poly> poly);
	bool InsertIfSpace(std::shared_ptr<Poly> poly);
	void GetAllIntersectingPolygons(std::shared_ptr<Poly> poly, std::vector<std::shared_ptr<Poly>> &container);

	void GetAllQuads(std::vector<std::shared_ptr<RBRect>> &container) const;
	void GetAllPoints(std::vector<std::shared_ptr<RPoint>> &container) const;
	void GetAllAABB(std::vector<std::shared_ptr<RRect>> &container) const;
	void GetAllCircles(std::vector<std::shared_ptr<RCircle>>& container) const;
	
	void PrintAllQuads() const;

	void AddRandomizedPoints(const int numPoints);
	
	const std::shared_ptr<RBRect>& GetBounds() const;
private:
	std::shared_ptr<RBRect> bounds;
	std::vector<std::shared_ptr<Poly>> polygons;
	QuadTree** Nodes;
};

