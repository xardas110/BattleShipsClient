#include "BoundingShapes.h"
#include <exception>
#include <iostream>
#include "Game.h"

using namespace DirectX;

void _vectorcall Box::SetCenter(glm::vec3 pos)
{
	Center = _mm_set_ps(0.f, pos.z, pos.y, pos.x);
	UpdateBounds();
}

void _vectorcall Box::SetExtents(glm::vec3 scale)
{
	Extents = _mm_set_ps(0.f, scale.z, scale.y, scale.x);
	UpdateBounds();
}

void Box::UpdateBounds()
{
	Bounds[0] = _mm_sub_ps(Center, Extents);
	Bounds[1] = _mm_add_ps(Center, Extents);
}

const glm::vec3& Rect::GetPosition() const
{
	return C;
}

const glm::vec3& Rect::GetScale() const
{
	return E;
}

const glm::mat4 Rect::GetTranslation() const
{
	glm::mat4 model(1.f);

	model[0][0] = E.x;
	model[1][1] = E.y;

	model[3][0] = C.x;
	model[3][1] = C.y;

	return model;
}

glm::vec3 Rect::GetMinBounds() const
{
	return C - E;
}

glm::vec3 Rect::GetMaxBounds() const
{
	return C + E;
}

void Rect::Print() const
{
	std::cout << "Centre: " << C.x << " " << C.y << " Extents: " << E.x << " " << E.y << std::endl;
}

bool Rect::Intersect(std::shared_ptr<Poly>& poly)
{
	switch (poly->GetType())
	{
	case Null:
		break;
	case Point:
		return IntersectPoint(std::dynamic_pointer_cast<RPoint>(poly));
	case Rectangle:
		return IntersectRect(std::dynamic_pointer_cast<RRect>(poly));
	case Circle:
		return IntersectCircle(std::dynamic_pointer_cast<RCircle>(poly));
	default:
		throw std::exception("Lack of intersection functions");
	}
	return false;
}

bool Rect::IntersectPoint(const std::shared_ptr<::Point> point) const
{
	std::cout << "intersect point running " << std::endl;
	const auto pointC = point->point - this->C;
	return pointC.x < this->E.x && pointC.x > -this->E.x && pointC.y < this->E.y && pointC.y > -this->E.y;
}

bool Rect::IntersectRect(const std::shared_ptr<Rect> rect) const
{
	const auto BC = rect->C - this->C;
	const auto BMin = BC - rect->E;
	const auto BMax = BC + rect->E;
	const auto AMin = -this->E;
	const auto AMax = this->E;

	for (auto i = 0; i < 3; i++)
	{
		if (BMax[i] < AMin[i] || BMin[i] > AMax[i])
		{
			std::cout << "intersect rect FAILED" << std::endl;
			return false;
		}
	}
	std::cout << "intersect rect returns true" << std::endl;
	return true;
}

bool Rect::IntersectCircle(const std::shared_ptr<::Circle> circle) const
{
	//compute closest point in box to point
	const auto PC = circle->C;
	const auto BMin = this->C - this->E;
	const auto BMax = this->C + this->E;

	::Point closestInBox{};
	for (auto i = 0; i < 3; i++)
	{
		if (PC[i] > BMin[i] && PC[i] < BMax[i])
			closestInBox.point[i] = PC[i];
		else
		{
			if (PC[i] < BMin[i])
				closestInBox.point[i] = BMin[i];
			else
				closestInBox.point[i] = BMax[i];
		}
	}

	return glm::distance(closestInBox.point, circle->C) < circle->R;
}

ORect::ORect()
	:Rect({0.f, 0.f,0.f}, {0.5f, 0.5f, 0.f}, Poly::Types::ORectangle), orient(glm::quat(0.f, {0.f, 0.f, 0.f}))
{
}

ORect::ORect(const glm::vec3& c, const glm::vec3& e, const glm::quat& orient)
	:Rect(c, e, Poly::Types::ORectangle), orient(orient)
{
}

void ORect::SetRotation(const float rotInDeg, const glm::vec3& axis)
{
	const glm::quat result = glm::identity<glm::quat>();
	orient = glm::rotate(result, glm::radians(rotInDeg), axis);
}

const glm::mat4 ORect::GetTranslation() const
{
	glm::mat4 model(1.f);
	const glm::mat4 rotationMatrix = glm::toMat4(orient);
	
	model = glm::translate(model, C);	
	model = glm::scale(model, E);
	model = model * rotationMatrix;
	
	return model;
}

const glm::mat4 Point::GetTranslation() const
{
	glm::mat4 model(1.f);
	model[3][0] = point.x;
	model[3][1] = point.y;
	return model;
}

bool Point::Intersect(std::shared_ptr<Poly>& poly)
{
	switch (poly->GetType())
	{
	case Null:
		break;
	case Rectangle:
		return IntersectRect(std::dynamic_pointer_cast<Rect>(poly));
	case Types::Point:
		return IntersectPoint(std::dynamic_pointer_cast<Point>(poly));
	case Circle:
		return IntersectCircle(std::dynamic_pointer_cast<::Circle>(poly));
	default:
		throw std::exception("Lack of intersection function");
	}
	return false;
}

bool Point::IntersectRect(const std::shared_ptr<Rect>& rect) const
{
	const auto pointC = point - rect->C;
	return pointC.x < rect->E.x && pointC.x > -rect->E.x && pointC.y < rect->E.y && pointC.y > -rect->E.y;
}

bool Point::IntersectPoint(const std::shared_ptr<Point>& point) const
{
	if (glm::distance(point->point, this->point) < point->pointThickness + this->pointThickness)
		return true;
	return false;
}

bool Point::IntersectCircle(const std::shared_ptr<::Circle>& circle) const
{
	return glm::distance(this->point, circle->C) < circle->R + this->pointThickness;
}

Circle::Circle(glm::vec3 centre, float radius)
	: Poly(Types::Circle), C(centre), R(radius)
{
}

const glm::mat4 Circle::GetTranslation() const
{
	glm::mat4 model(1.f);
	model[0][0] = R;
	model[1][1] = R;
	model[2][2] = R;
	model[3][0] = C.x;
	model[3][1] = C.y;
	model[3][2] = C.z;
	return model;
}

bool Circle::Intersect(std::shared_ptr<Poly>& poly)
{
	switch (poly->GetType())
	{
	case Null:
		break;
	case Point:
		return IntersectPoint(std::dynamic_pointer_cast<::Point>(poly));
	case Types::Circle:
		return IntersectCircle(std::dynamic_pointer_cast<Circle>(poly));
	case Rectangle:
		return IntersectRect(std::dynamic_pointer_cast<RRect>(poly));
	default:
		throw std::exception("lacking intersection method");
	}
	return false;
}

bool Circle::IntersectPoint(std::shared_ptr<::Point> point) const
{
	return glm::distance(point->point, this->C) < this->R + point->pointThickness;
}

bool Circle::IntersectRect(const std::shared_ptr<Rect>& rect) const
{
	//compute closest point in box to point
	const auto PC = this->C;
	const auto BMin = rect->C - rect->E;
	const auto BMax = rect->C + rect->E;

	::Point closestInBox{};
	for (auto i = 0; i < 3; i++)
	{
		if (PC[i] > BMin[i] && PC[i] < BMax[i])
			closestInBox.point[i] = PC[i];
		else
		{
			if (PC[i] < BMin[i])
				closestInBox.point[i] = BMin[i];
			else
				closestInBox.point[i] = BMax[i];
		}
	}

	return glm::distance(closestInBox.point, this->C) < this->R;
}

bool Circle::IntersectCircle(const std::shared_ptr<Circle> circle) const
{
	return glm::distance(circle->C, this->C) < this->R + circle->R;
}

Poly::Poly(Types type)
	: Type(type)
{
}

bool Poly::Intersect(std::shared_ptr<Poly>& poly)
{
	throw std::exception("Poly intersect function not overriden");
}

PolyRender* Poly::GetRenderSettings()
{
	return nullptr;
}

const glm::mat4 Poly::GetTranslation() const
{
	throw std::exception("Shape is lacking a translation");
}

Poly::Types Poly::GetType() const
{
	return Type;
}

PolyRender::PolyRender()
	: ambient(0.2f, 0.2f, 0.2f), diffuse(0.0f, 0.5f, 0.0f), specular(0.0f, 0.5f, 0.0f), renderMode(0x0004),
	  shininess(64.f)
{
}

void PolyRender::SetDiffuse(const glm::vec3& rgb)
{
	this->diffuse = rgb;
}

void PolyRender::SetSpecular(const glm::vec3& rgb)
{
	this->specular = rgb;
}

void PolyRender::SetShininess(const float shininess)
{
	this->shininess = shininess;
}

RCircle::RCircle()
	: Circle({0.f, 0.f, 0.f}, 1.f)
{
	Game::AddToDrawList(this);
}

RCircle::RCircle(const glm::vec3& centre, const float radius)
	: Circle(centre, radius)
{
	Game::AddToDrawList(this);
}

RCircle::~RCircle()
{
	Circle::~Circle();
	Game::RemoveFromDrawList(this);
}

PolyRender* RCircle::GetRenderSettings()
{
	return this;
}

std::shared_ptr<RCircle> RCircle::Create()
{
	return std::shared_ptr<RCircle>(new RCircle);
}

std::shared_ptr<RCircle> RCircle::Create(const glm::vec3& c, const float r)
{
	return std::shared_ptr<RCircle>(new RCircle(c,r));
}

RRect::RRect()
{
	Game::AddToDrawList(this);
}

RRect::RRect(const glm::vec3& c, const glm::vec3& e)
	: Rect(c, e)
{
	Game::AddToDrawList(this);
}

RRect::~RRect()
{
	Rect::~Rect();
	Game::RemoveFromDrawList(this);
}

PolyRender* RRect::GetRenderSettings()
{
	return this;
}

std::shared_ptr<RRect> RRect::Create()
{
	return std::shared_ptr<RRect>(new RRect);
}

std::shared_ptr<RRect> RRect::Create(const glm::vec3& c, const glm::vec3& e)
{
	return std::shared_ptr<RRect>(new RRect(c,e));
}

bool RRect::Intersect(std::shared_ptr<Poly>& poly)
{
	throw std::exception("Not implemented");
}

bool ORect::IntersectCircle(const std::shared_ptr<::Circle> circle) const
{
	throw std::exception("Not implemented");
}

bool ORect::IntersectRect(const std::shared_ptr<::Rect> rect) const
{
	throw std::exception("Not implemented");
}

bool ORect::IntersectPoint(const std::shared_ptr<::Point> point) const
{
	throw std::exception("Not implemented");
}

RPoint::RPoint()
{
	renderMode = 0x0000;
	Game::AddToDrawList(this);
}

RPoint::RPoint(const glm::vec3& p)
	: Point(p)
{
	renderMode = 0x0000;
	Game::AddToDrawList(this);
}

RPoint::~RPoint()
{
	Game::RemoveFromDrawList(this);
}

std::shared_ptr<RPoint> RPoint::Create()
{
	return std::shared_ptr<RPoint>(new RPoint);
}

std::shared_ptr<RPoint> RPoint::Create(const glm::vec3& p)
{
	return std::shared_ptr<RPoint>(new RPoint(p));
}

PolyRender* RPoint::GetRenderSettings()
{
	return this;
}

RBRect::RBRect()
{
	renderMode = 0x0002;
	Game::AddToDrawList(this);
}

RBRect::RBRect(const glm::vec3& c, const glm::vec3& e)
	: Rect(c, e)
{
	renderMode = 0x0002;
	Game::AddToDrawList(this);
}

RBRect::~RBRect()
{
	Rect::~Rect();
	Game::RemoveFromDrawList(this);
}

std::shared_ptr<RBRect> RBRect::Create()
{
	return std::shared_ptr<RBRect>(new RBRect);
}

std::shared_ptr<RBRect> RBRect::Create(const glm::vec3& c, const glm::vec3& e)
{
	return  std::shared_ptr<RBRect>(new RBRect(c, e));
}


bool _vectorcall OrientedBox::Intersect(const OrientedBox& OB)
{
	FXMVECTOR& A_E = this->E;
	FXMVECTOR& B_E = OB.E;
	XMMATRIX R{};
	XMVECTOR T{};
	{
		//Loading variables
		FXMVECTOR& A_C = this->C;
		FXMVECTOR& B_C = OB.C;

		/*Transposing B_U to make calculations inside A's cordinate space*/
		//* Transforming B into A's cordinate system*//
#ifdef QUATERNION
		{
			FXMVECTOR Q = XMQuaternionMultiply(this->QUAT, OB.invQUAT);
			R = XMMatrixRotationQuaternion(Q);
		}
		T = XMVector3InverseRotate(XMVectorSubtract(B_C, A_C), this->QUAT);
#else
		FXMMATRIX& A_U = this->R;
		FXMMATRIX& B_U = XMMatrixTranspose(OB.R);
		FXMMATRIX& A_UT = XMMatrixTranspose(this->R);
		T = B_C - A_C;
		T = XMVector3Transform(T, A_UT);
		R = XMMatrixMultiply(A_U, B_U);
#endif // QUATERION
	}
	auto NoIntersect = [](const auto& RA, const auto& RB, const auto& absT)
	{
		return !XMVector3Less(absT, XMVectorAdd(RA, RB));
	};
	constexpr XMVECTOR epsilonVec = {FLT_EPSILON, FLT_EPSILON, FLT_EPSILON, FLT_EPSILON};
	FXMMATRIX absR = {
		XMVectorAdd(XMVectorAbs(R.r[0]), epsilonVec), XMVectorAdd(XMVectorAbs(R.r[1]), epsilonVec),
		XMVectorAdd(XMVectorAbs(R.r[2]), epsilonVec), R.r[3]
	};

	XMVECTOR absT = XMVectorAbs(T);
	XMVECTOR RA, RB, B_U1, B_U2, A_E1, A_E2;
	{
		{
			FXMMATRIX absRT = XMMatrixTranspose(absR);
			/*Projecting into A's cordinate system*/
			RB = XMVector3Transform(B_E, absRT);
			if (NoIntersect(A_E, RB, absT)) return false;
		}

		/*Projecting into B's cordinate system*/
		RA = XMVector3Transform(A_E, absR);
		absT = XMVectorAbs(XMVector3Transform(T, R));
		if (NoIntersect(RA, B_E, absT)) return false;
		B_U1 = XMVectorSwizzle<2U, 2U, 1U, 3U>(absR.r[0]);
		B_U2 = XMVectorSwizzle<1U, 0U, 0U, 3U>(absR.r[0]);
		A_E1 = XMVectorSplatY(A_E); //XMVectorSwizzle<1U, 1U, 1U, 3U>(A_E);
		A_E2 = XMVectorSplatZ(A_E); //XMVectorSwizzle<2U, 2U, 2U, 3U>(A_E);
		RA = XMVectorAdd(XMVectorMultiply(A_E1, absR.r[2]), XMVectorMultiply(A_E2, absR.r[1]));
	}
	/*Crossing A with B to check edge scenarios*/
	// Testing AU0 x BU0, AU0 x BU1, AU0 x BU2

	const auto B_E1 = XMVectorSwizzle<1U, 0U, 0U, 3U>(B_E);
	const auto B_E2 = XMVectorSwizzle<2U, 2U, 1U, 3U>(B_E);

	const auto T1 = XMVectorSplatZ(T);
	const auto T2 = XMVectorSplatY(T);


	RB = XMVectorAdd(XMVectorMultiply(B_E1, B_U1), XMVectorMultiply(B_E2, B_U2));
	absT = XMVectorAbs(XMVectorSubtract(XMVectorMultiply(T1, R.r[1]), XMVectorMultiply(T2, R.r[2])));
	if (NoIntersect(RA, RB, absT)) return false;

	// Testing AU1 x BU0, AU1 x BU1, AU1 x BU2
	FXMVECTOR T3 = XMVectorSplatX(T);
	A_E1 = XMVectorSplatX(A_E); //XMVectorSwizzle<0U, 0U, 0U, 3U>(A_E);
	B_U1 = XMVectorSwizzle<2U, 2U, 1U, 3U>(absR.r[1]);
	B_U2 = XMVectorSwizzle<1U, 0U, 0U, 3U>(absR.r[1]);
	RA = XMVectorAdd(XMVectorMultiply(A_E1, absR.r[2]), XMVectorMultiply(A_E2, absR.r[0]));
	RB = XMVectorAdd(XMVectorMultiply(B_E1, B_U1), XMVectorMultiply(B_E2, B_U2));
	absT = XMVectorAbs(XMVectorSubtract(XMVectorMultiply(T3, R.r[2]), XMVectorMultiply(T1, R.r[0])));
	if (NoIntersect(RA, RB, absT)) return false;

	// Testing AU2 x BU0, AU2 x BU1, AU2 x BU2
	A_E2 = XMVectorSplatY(A_E); //XMVectorSwizzle<1U, 1U, 1U, 3U>(A_E);
	B_U1 = XMVectorSwizzle<2U, 2U, 1U, 3U>(absR.r[2]);
	B_U2 = XMVectorSwizzle<1U, 0U, 0U, 3U>(absR.r[2]);
	RA = XMVectorAdd(XMVectorMultiply(A_E1, absR.r[1]), XMVectorMultiply(A_E2, absR.r[0]));
	RB = XMVectorAdd(XMVectorMultiply(B_E1, B_U1), XMVectorMultiply(B_E2, B_U2));
	absT = XMVectorAbs(XMVectorSubtract(XMVectorMultiply(T2, R.r[0]), XMVectorMultiply(T3, R.r[1])));
	if (NoIntersect(RA, RB, absT)) return false;
	return true;
}


PolyRender* RBRect::GetRenderSettings()
{
	return this;
}

RORect::~RORect()
{
	Game::RemoveFromDrawList(this);
}

PolyRender* RORect::GetRenderSettings()
{
	return this;
}

std::shared_ptr<RORect> RORect::Create()
{
	return std::shared_ptr<RORect>(new RORect);
}

std::shared_ptr<RORect> RORect::Create(const glm::vec3& c, const glm::vec3& e, const glm::quat& orient)
{
	return std::shared_ptr<RORect>(new RORect(c, e, orient));
}

RORect::RORect()
	:ORect()
{
	Game::AddToDrawList(this);
}

RORect::RORect(const glm::vec3& c, const glm::vec3& e, const glm::quat& orient)
	:ORect(c, e, orient)
{
	Game::AddToDrawList(this);
}

Segment::Segment()
	:Poly(Poly::Types::Segment), A({0.f, 0.f, 0.f}), B({0.f, 1.f, 0.f})
{
}

Segment::Segment(const glm::vec3& A, const glm::vec3& B)
	: Poly(Poly::Types::Segment), A(A), B(B)
{	
}

const glm::mat4 Segment::GetTranslation() const
{
	glm::mat4 model(1.f);
	
	const glm::vec3 lineMiddle = (A + B) * 0.5f;
	const glm::vec3 lineVector = B - A;
	const float lineLength = glm::length(lineVector);
	const glm::vec3 lineDirection = lineVector / lineLength;
	const float lineHalfLength = lineLength * 0.5f;

	
	model = glm::translate(model, lineMiddle);
	model = glm::scale(model, glm::vec3(lineHalfLength, lineHalfLength, lineHalfLength));
	//model = model * glm::lookAt(A, B, { 0.f, 0.f, 1.f });
		
	return model;
}

RSegment::RSegment()
	:Segment()
{
	renderMode = 0x0001;
	Game::AddToDrawList(this);
}

RSegment::RSegment(const glm::vec3& A, const glm::vec3& B)
	:Segment(A, B)
{
	renderMode = 0x0001;
	Game::AddToDrawList(this);
}

RSegment::~RSegment()
{
	Game::RemoveFromDrawList(this);
}

PolyRender* RSegment::GetRenderSettings()
{
	return this;
}

std::shared_ptr<RSegment> RSegment::Create()
{
	return std::shared_ptr<RSegment>(new RSegment);
}

std::shared_ptr<RSegment> RSegment::Create(const glm::vec3& A, const glm::vec3& B)
{
	return std::shared_ptr<RSegment>(new RSegment(A, B));
}
