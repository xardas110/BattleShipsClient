#include "QuadTree.h"
#include <iostream>
#include <ostream>
#include "Macros.h"
#include "../Engine/BoundingShapes.h"
#include "../Engine/Collision.h"

struct Poly;

QuadTree::QuadTree(const std::shared_ptr<RBRect>& rect)
	: bounds(rect)
{
	Nodes = new QuadTree * [Size];

	for (auto i = 0; i < Size; i++)
	{
		Nodes[i] = nullptr;
	}
};


bool QuadTree::IsSubDivided() const
{
	return Nodes[0] != nullptr;
}

void QuadTree::SubDivide()
{
	if (IsSubDivided())
		return;

	const auto HalfExtent = bounds->E * 0.5f;
	const auto &HalfExtentX = HalfExtent.x;
	const auto &HalfExtentY = HalfExtent.y;

	Nodes[NW] = new QuadTree
	(RBRect::Create(
		glm::vec3(bounds->C.x - HalfExtentX, bounds->C.y + HalfExtentY, 0.f),
		HalfExtent
	));

	Nodes[NE] = new QuadTree
	(RBRect::Create(
		glm::vec3(bounds->C.x + HalfExtentX, bounds->C.y + HalfExtentY, 0.f),
		HalfExtent
	));

	Nodes[SW] = new QuadTree
	(RBRect::Create(
		glm::vec3(bounds->C.x - HalfExtentX, bounds->C.y - HalfExtentY, 0.f),
		HalfExtent
	));

	Nodes[SE] = new QuadTree
	(RBRect::Create(
		glm::vec3(bounds->C.x + HalfExtentX, bounds->C.y - HalfExtentY, 0.f),
		HalfExtent
	));
	
}

void QuadTree::SubDivide(const int n)
{
	if (n >= 0)
	{
		if (IsSubDivided())
		{
			for (auto i = 0; i<Size; i++)
			{
				Nodes[i]->SubDivide(n - 1);
			}
		}
		else
		{
			SubDivide();
			SubDivide(n - 1);
		}
	}
}

bool QuadTree::Insert(std::shared_ptr<Poly> poly)
{
	if (!Collision::Intersect(this->bounds.get(), poly.get()))
	{
		std::cout << "Bounds failing" << std::endl;
		return false;
	}

	if (polygons.size() < MAX_POINTS_PR_QUAD)
	{	
		polygons.push_back(poly);
	}
	else
	{	
		if (!IsSubDivided())
			SubDivide();			
		
		for (auto i = 0; i < Size; i++)
			Nodes[i]->Insert(poly);
	}
}

bool QuadTree::InsertIfSpace(std::shared_ptr<Poly> poly)
{
	std::vector<std::shared_ptr<Poly>> possibleIntersectionContainer;
	GetAllIntersectingPolygons(poly, possibleIntersectionContainer);
	
	for (const auto &polygon : possibleIntersectionContainer)
		if (Collision::Intersect(polygon.get(), poly.get()))
			return false;

	std::cout << "possible intersecting polygons size: " << possibleIntersectionContainer.size() << std::endl;
	
	return Insert(poly);
}

void QuadTree::GetAllIntersectingPolygons(std::shared_ptr<Poly> poly, std::vector<std::shared_ptr<Poly>> &container)
{
	
	if (!Collision::Intersect(this->bounds.get(), poly.get()))
		return;

	for (const auto polygon : polygons)
		container.push_back(polygon);

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->GetAllIntersectingPolygons(poly, container);
}

void QuadTree::GetAllQuads(std::vector<std::shared_ptr<RBRect>>& container) const
{
	container.push_back(bounds);

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)	
				Nodes[i]->GetAllQuads(container);
}

void QuadTree::GetAllPoints(std::vector<std::shared_ptr<RPoint>>& container) const
{
	for (const auto point : polygons)
		if (point->GetType() == Poly::Types::Point)
			container.push_back(std::dynamic_pointer_cast<::RPoint>(point));

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->GetAllPoints(container);
}

void QuadTree::GetAllAABB(std::vector<std::shared_ptr<RRect>>& container) const
{
	for (const auto point : polygons)
		if (point->GetType() == Poly::Types::Rectangle)
			container.push_back(std::dynamic_pointer_cast<::RRect>(point));

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->GetAllAABB(container);
}

void QuadTree::GetAllCircles(std::vector<std::shared_ptr<RCircle>>& container) const
{
	for (const auto point : polygons)
		if (point->GetType() == Poly::Types::Circle)
			container.push_back(std::dynamic_pointer_cast<::RCircle>(point));

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->GetAllCircles(container);
}

void QuadTree::PrintAllQuads() const
{
	bounds->Print();

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->PrintAllQuads();		
}

void QuadTree::AddRandomizedPoints(const int numPoints)
{
	for (auto i = 0; i < numPoints; i++)
	{
		const auto floatScale = 0.001f;
		const auto scale = 1000.f;
		const auto randVal = 8.f * scale;
		float x = (rand() % (int)randVal);
		float y = (rand() % (int)randVal);
		x -= randVal * 0.5f;
		y -= randVal * 0.5f;
		auto p = RPoint::Create({ x * floatScale, y * floatScale, 0.f });
		p->GetRenderSettings()->SetDiffuse({ 1.f, 0.f, 0.f });
		p->GetRenderSettings()->SetSpecular({ 1.f, 0.f, 0.f });
		Insert(p);
	}
}

const std::shared_ptr<RBRect> &QuadTree::GetBounds() const
{
	return bounds;
}
