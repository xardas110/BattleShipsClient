#include "QuadTree.h"
#include "Macros.h"

QuadTree::QuadTree(const Rect& rect)
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

	const auto HalfExtent = bounds.E * 0.5f;
	const auto &HalfExtentX = HalfExtent.x;
	const auto &HalfExtentY = HalfExtent.y;

	Nodes[NW] = new QuadTree
	({
		glm::vec3(bounds.C.x - HalfExtentX, bounds.C.y + HalfExtentY, 0.f),
		HalfExtent
	});

	Nodes[NE] = new QuadTree
	({
		glm::vec3(bounds.C.x + HalfExtentX, bounds.C.y + HalfExtentY, 0.f),
		HalfExtent
	});

	Nodes[SW] = new QuadTree
	({
		glm::vec3(bounds.C.x - HalfExtentX, bounds.C.y - HalfExtentY, 0.f),
		HalfExtent
	});

	Nodes[SE] = new QuadTree
	({
		glm::vec3(bounds.C.x + HalfExtentX, bounds.C.y - HalfExtentY, 0.f),
		HalfExtent
	});
	
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

void QuadTree::Insert(Poly* poly)
{
	if (!this->bounds.Intersect(poly))
	{
		std::cout << "Bounds failing" << std::endl;
		return;
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

void QuadTree::InsertIfSpace(Poly* poly)
{
	std::vector<Poly*> possibleIntersectionContainer;
	GetAllIntersectingPolygons(poly, possibleIntersectionContainer);
	
	for (auto* polygon : possibleIntersectionContainer)
		if (polygon->Intersect(poly))
			return;

	std::cout << "possible intersecting polygons size: " << possibleIntersectionContainer.size() << std::endl;
	
	Insert(poly);
}

void QuadTree::GetAllIntersectingPolygons(Poly* poly, std::vector<Poly*> &container)
{
	if (!this->bounds.Intersect(poly))
		return;

	for (auto* polygon : polygons)
		container.push_back(polygon);

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->GetAllIntersectingPolygons(poly, container);
}

void QuadTree::GetAllQuads(std::vector<Rect>& container) const
{
	container.push_back(bounds);

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)	
				Nodes[i]->GetAllQuads(container);
}

void QuadTree::GetAllPoints(std::vector<Point*>& container) const
{
	for (auto *point : polygons)
		if (point->GetType() == Poly::Types::Point)
			container.push_back(dynamic_cast<::Point*>(point));

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->GetAllPoints(container);
}

void QuadTree::GetAllAABB(std::vector<Rect*>& container) const
{
	for (auto* point : polygons)
		if (point->GetType() == Poly::Types::Rectangle)
			container.push_back(dynamic_cast<::Rect*>(point));

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->GetAllAABB(container);
}

void QuadTree::GetAllCircles(std::vector<Circle*>& container) const
{
	for (auto* point : polygons)
		if (point->GetType() == Poly::Types::Circle)
			container.push_back(dynamic_cast<::Circle*>(point));

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->GetAllCircles(container);
}

void QuadTree::PrintAllQuads() const
{
	bounds.Print();

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->PrintAllQuads();		
}

const Rect& QuadTree::GetBounds() const
{
	return bounds;
}
