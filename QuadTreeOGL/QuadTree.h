#pragma once
#include <iostream>

#include "../include/glm/glm.hpp"
#include "../include/glm/gtx/transform.hpp"
#include <vector>

#define MAX_POINTS_PR_QUAD 4

struct Rect
{
	Rect(glm::vec3 c, glm::vec3 e)
		: C(c), E(e){}
	
	glm::vec3 C, E;

	const glm::vec3 &GetPosition() const
	{
		return C;
	};

	const glm::vec3& GetScale() const
	{
		return E;
	};

	glm::mat4 GetTranslation() const
	{
		glm::mat4 model(1.f);

		model[0][0] = E.x;
		model[1][1] = E.y;

		model[3][0] = C.x;
		model[3][1] = C.y;
		
		return model;
	}
	void Print() const
	{
		std::cout << "Centre: " << C.x << " " << C.y << " Extents: " << E.x << " " << E.y << std::endl;
	}
	
};

class QuadTree
{
	enum Coord
	{
		NW, NE, SE, SW, Size
	};

public:
	QuadTree(const Rect& rect)
		: bounds(rect)
	{
		Nodes = new QuadTree *[Size];
		
		for (auto i = 0; i< Size; i++)
		{
			Nodes[i] = nullptr;
		}
	};

	bool IsSubDivided() const;
	
	Rect bounds;
	QuadTree** Nodes;

	void SubDivide();
	void SubDivide(const int n);
	
	void GetAllNodes(std::vector<Rect>& container) const;
	void PrintAllNodes() const;
};

