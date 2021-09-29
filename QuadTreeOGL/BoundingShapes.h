#pragma once
#include <iostream>

#include "../include/glm/glm.hpp"

struct Point;
struct Rect;

struct Poly
{
	enum Types
	{
		Point, Rectangle, Null
	};

	Poly(Types type);

	virtual bool Intersect(Poly* poly);
	Types GetType() const;
private:
	Types Type = Null;
};

struct Rect : public Poly
{
	Rect(glm::vec3 c, glm::vec3 e): C(c), E(e), Poly(Poly::Types::Rectangle) {}

	const glm::vec3& GetPosition() const;
	
	const glm::vec3& GetScale() const;

	glm::mat4 GetTranslation() const;

	glm::vec3 GetMinBounds() const;
	glm::vec3 GetMaxBounds() const;
	
	void Print() const;

	glm::vec3 C, E;
	//virtual bool Intersect(const Poly* poly);
	virtual bool Intersect(::Poly *poly);
private:
	bool IntersectPoint(const ::Point* point) const;
	bool IntersectRect(const ::Rect* rect) const;
};

struct Point : public Poly
{
	Point() : point{ 0.f, 0.f, 0.f }, Poly(Poly::Types::Point) {};
	Point(glm::vec3 point) : point(point), Poly(Poly::Types::Point) {};

	glm::mat4 GetTranslation() const;
	glm::vec3 point;

	virtual bool Intersect(::Poly* poly) override;
private:
	bool IntersectRect(const Rect* rect) const;
};