#pragma once
#include <iostream>

#include "../include/glm/glm.hpp"

struct Point;

struct Rect
{
	Rect(glm::vec3 c, glm::vec3 e): C(c), E(e) {}

	const glm::vec3& GetPosition() const;
	
	const glm::vec3& GetScale() const;

	glm::mat4 GetTranslation() const;

	glm::vec3 GetMinBounds() const;
	glm::vec3 GetMaxBounds() const;
	
	void Print() const;

	bool Intersect(const Point& point) const;

	glm::vec3 C, E;
};

struct Point
{
	Point() : point{ 0.f, 0.f, 0.f } {};
	Point(glm::vec3 point) : point(point) {};

	glm::mat4 GetTranslation() const;
	
	bool Intersect(const Rect& rect) const;

	glm::vec3 point;
};
