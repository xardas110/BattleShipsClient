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
	const auto HalfExtentX = HalfExtent.x;
	const auto HalfExtentY = HalfExtent.y;

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

void QuadTree::Insert(const Point& point)
{
	if (!this->bounds.Intersect(point))
		return;

	auto loopInsert = [&point, this]()
	{
		for (auto i = 0; i < Size; i++)
			Nodes[i]->Insert(point);
	};
	
	if (points.size() < MAX_POINTS_PR_QUAD)
	{	
		points.push_back(point);
	}
	else
	{
		
		if (IsSubDivided())
		{
			loopInsert();
		}
		else
		{
			SubDivide();
			loopInsert();
		}
	}
}

void QuadTree::GetAllQuads(std::vector<Rect>& container) const
{
	container.push_back(bounds);

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)	
				Nodes[i]->GetAllQuads(container);
}

void QuadTree::GetAllPoints(std::vector<Point>& container) const
{
	for (const auto &point : points)
		container.push_back(point);

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
			Nodes[i]->GetAllPoints(container);
}

void QuadTree::PrintAllQuads() const
{

	bounds.Print();

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
		{
			Nodes[i]->PrintAllQuads();
		}
}

const Rect& QuadTree::GetBounds() const
{
	return bounds;
}
