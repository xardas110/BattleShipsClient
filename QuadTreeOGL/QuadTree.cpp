#include "QuadTree.h"

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

void QuadTree::GetAllNodes(std::vector<Rect>& container) const
{
	container.push_back(bounds);

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)	
				Nodes[i]->GetAllNodes(container);
}

void QuadTree::PrintAllNodes() const
{

	bounds.Print();

	if (IsSubDivided())
		for (auto i = 0; i < Size; i++)
		{
			Nodes[i]->PrintAllNodes();
		}
}
