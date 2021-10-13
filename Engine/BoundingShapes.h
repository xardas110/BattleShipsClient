#pragma once
#include <DirectXMath.h>

#include <xmmintrin.h>
#include <memory>

#include "../include/glm/gtx/quaternion.hpp"
#include "../include/glm/fwd.hpp"
#include "../include/glm/vec3.hpp"


struct PolyRender;
struct Point;
struct Rect;
struct Circle;


_declspec(align(16)) struct Box
{
	Box()
		:Center{ 0.f, 0.f, 0.f, 0.f }, Extents{ 0.5f, 0.5f, 0.0f, 0.f }
	{
		UpdateBounds();
	}
	Box(glm::vec3 center, glm::vec3 extents)
		:Center{ center.x, center.y, center.z, 0.f }, Extents{ extents.x, extents.y, extents.z, 0.f }
	{
		UpdateBounds();
	}
	__m128				Center;
	__m128				Extents;
	__m128				Bounds[2U];

	void _vectorcall	SetCenter(glm::vec3 pos);
	void _vectorcall	SetExtents(glm::vec3 pos);
	void _vectorcall	UpdateBounds();
};

#define QUATERNION
struct OrientedBox
{

#ifdef QUATERNION
	OrientedBox(DirectX::XMVECTOR C, DirectX::XMVECTOR E, DirectX::XMVECTOR QUAT)
		:QUAT(QUAT), invQUAT(DirectX::XMQuaternionConjugate(QUAT)), C(C), E(E)
	{
	};
	OrientedBox(const glm::vec3 &C, const glm::vec3& E, const glm::quat &QUAT)
	{
		this->QUAT.m128_f32[0] = QUAT.x;
		this->QUAT.m128_f32[1] = QUAT.y;
		this->QUAT.m128_f32[2] = QUAT.z;
		this->QUAT.m128_f32[3] = QUAT.w;
		
		this->C.m128_f32[0] = C.x;
		this->C.m128_f32[1] = C.y;
		this->C.m128_f32[2] = C.z;
		this->C.m128_f32[3] = 0.f;

		this->E.m128_f32[0] = E.x;
		this->E.m128_f32[1] = E.y;
		this->E.m128_f32[2] = E.z;
		this->E.m128_f32[3] = 0.f;

		invQUAT = DirectX::XMQuaternionConjugate(this->QUAT);
	};
	DirectX::XMVECTOR QUAT, invQUAT;
#else
	OrientedBox(DirectX::XMVECTOR C, DirectX::XMVECTOR E, DirectX::XMMATRIX R)
		:R(R), C(C), E(E)
	{
	};
	DirectX::XMMATRIX R;
#endif // QUATERION

	DirectX::XMVECTOR C, E;
public:
	bool _vectorcall Intersect(_In_ const OrientedBox& OB);
};

struct Poly
{
	enum Types
	{
		Null = -1, RBRect, Point, Rectangle, Circle, ORectangle, Segment, Capsule2D, Size
	};

	Poly(Types type);

	/// <param name="poly">Polygon to test intersection against</param>
	/// <returns>true if intersects</returns>
	virtual bool Intersect(std::shared_ptr<Poly>& poly);
	
	/// <summary>
	/// For polygons that wants to be rendered need to have render settings in their derived class.
	/// </summary>
	/// <returns>Render settings for the polygon. Bounding volumes will just return nullptr</returns>
	virtual PolyRender *GetRenderSettings();

	const virtual glm::mat4 GetTranslation() const;
	
	Types GetType() const;

private:
	Types Type = Null;
};

struct Segment : Poly
{
	Segment();
	Segment(const glm::vec3 &A, const glm::vec3 &B);

	Segment(const Segment&) = default;
	Segment(Segment&&) = default;

	virtual const glm::mat4 GetTranslation() const override;
	
	Segment& operator= (const Segment &) = default;
	Segment& operator= (Segment&&) = default;
	
	virtual ~Segment() = default;
	glm::vec3 A, B;
};

struct Capsule : Segment
{
	float r;
	
	//virtual ~Capsule() = default;
};

struct Rect : Poly
{
	Rect() : Poly(Poly::Types::Rectangle), C{ 0.f, 0.f, 0.f }, E(0.2f, 0.2f, 0.2f) {}
	Rect(glm::vec3 c, glm::vec3 e) : Poly(Poly::Types::Rectangle), C(c), E(e) {}

	const glm::vec3& GetPosition() const;

	const glm::vec3& GetScale() const;

	virtual const glm::mat4 GetTranslation() const override;

	glm::vec3 GetMinBounds() const;

	glm::vec3 GetMaxBounds() const;

	void Print() const;
	
	glm::vec3 C, E;
	//virtual bool Intersect(const Poly* poly);
	bool Intersect(std::shared_ptr<::Poly>& poly) override;
protected:

	Rect(Poly::Types type) : Poly(type), C{ 0.f, 0.f, 0.f }, E(0.2f, 0.2f, 0.2f) {}
	Rect(glm::vec3 c, glm::vec3 e, Poly::Types type) : Poly(type), C(c), E(e) {}
	
	virtual ~Rect() = default;
	virtual bool IntersectPoint(const std::shared_ptr<::Point> point) const;
	virtual bool IntersectRect(const std::shared_ptr <::Rect> rect) const;
	virtual bool IntersectCircle(const std::shared_ptr <::Circle> circle) const;
};

struct ORect: Rect
{
	ORect();
	ORect(const glm::vec3 &c, const glm::vec3 &e, const glm::quat &orient);
	glm::quat orient;

	void SetRotation(const float rotInDeg, const glm::vec3& axis);
	virtual const glm::mat4 GetTranslation() const override;
protected:
	bool IntersectCircle(const std::shared_ptr<::Circle> circle) const override;
	bool IntersectRect(const std::shared_ptr<::Rect> rect) const override;
	bool IntersectPoint(const std::shared_ptr<::Point> point) const override;
};


struct Point : Poly
{
	Point() : Poly(Poly::Types::Point), point{ 0.f, 0.f, 0.f } {}
	Point(glm::vec3 point) :Poly(Poly::Types::Point), point(point) {}

	const glm::mat4 GetTranslation() const override;
	
	glm::vec3 point;
	float pointThickness{ .01f };

	bool Intersect(std::shared_ptr<::Poly>& poly) override;

	virtual ~Point() = default;

private:
	bool IntersectRect(const std::shared_ptr<Rect>& rect) const;
	bool IntersectPoint(const std::shared_ptr<Point>& point) const;
	bool IntersectCircle(const std::shared_ptr<::Circle>& circle) const;
};


struct Circle : Poly
{
	Circle(glm::vec3 centre, float radius);

	const glm::mat4 GetTranslation() const override;

	bool Intersect(std::shared_ptr<Poly>& poly) override;

	bool IntersectPoint(const std::shared_ptr<::Point> point) const;

	bool IntersectRect(const std::shared_ptr<::Rect>& rect) const;

	bool IntersectCircle(const std::shared_ptr<::Circle> circle) const;

	glm::vec3 C;
	float R;

	virtual ~Circle() = default;	
};

struct PolyRender
{
	PolyRender();

	void SetDiffuse(const glm::vec3& rgb);
	void SetSpecular(const glm::vec3& rgb);
	void SetShininess(const float shininess);

	glm::vec3 ambient, diffuse, specular;
	int renderMode;
	float shininess;
};

struct RSegment : Segment, PolyRender
{
private:
	RSegment();
	RSegment(const glm::vec3& A, const glm::vec3& B);
public:
	RSegment(const RSegment&) = delete;
	RSegment(RSegment&&) = delete;

	RSegment& operator= (const RSegment&) = delete;
	RSegment& operator= (RSegment&&) = delete;

	~RSegment() override;

	PolyRender* GetRenderSettings() final override;

	static  std::shared_ptr<RSegment> Create();
	static std::shared_ptr<RSegment> Create(const glm::vec3& A, const glm::vec3& B);
};

struct RCircle : Circle, PolyRender
{
private:
	RCircle();
	RCircle(const glm::vec3 &c, const float r);
public:
	RCircle(const RCircle&) = delete;
	RCircle(RCircle&&) = delete;

	RCircle& operator= (const RCircle&) = delete;
	RCircle& operator= (RCircle&&) = delete;

	~RCircle() override;

	PolyRender* GetRenderSettings() final override;

	static  std::shared_ptr<RCircle> Create();
	static std::shared_ptr<RCircle> Create(const glm::vec3& c, const float r);
};

struct RRect : Rect, PolyRender
{
private:
	RRect();
	RRect(const glm::vec3& c, const glm::vec3& e);
public:
	RRect(const RRect&) = delete;
	RRect(RRect&&) = delete;

	RRect& operator= (const RRect&) = delete;
	RRect& operator= (RRect&&) = delete;

	~RRect() override;

	PolyRender* GetRenderSettings() final override;

	static std::shared_ptr<RRect> Create();
	static std::shared_ptr<RRect> Create(const glm::vec3& c, const glm::vec3& e);

	bool Intersect(std::shared_ptr<Poly>& poly) override;

};

struct RORect : ORect, PolyRender
{
private:
	RORect();
	RORect(const glm::vec3& c, const glm::vec3& e, const glm::quat &orient);
public:
	RORect(const RRect&) = delete;
	RORect(RRect&&) = delete;

	RORect& operator= (const RORect&) = delete;
	RORect& operator= (RORect&&) = delete;

	~RORect() override;
	
	PolyRender* GetRenderSettings() override;

	static std::shared_ptr<RORect> Create();
	static std::shared_ptr<RORect> Create(const glm::vec3& c, const glm::vec3& e, const glm::quat &orient);
};

struct RBRect : Rect, PolyRender
{
private:
	RBRect();
	RBRect(const glm::vec3& c, const glm::vec3& e);
public:
	RBRect(const RBRect&) = delete;
	RBRect(RBRect&&) noexcept = delete;

	RBRect& operator= (const RBRect&) = delete;
	RBRect& operator= (RBRect&&) = delete;

	~RBRect() override;
	
	PolyRender* GetRenderSettings() final override;

	static std::shared_ptr<RBRect> Create();
	static std::shared_ptr<RBRect> Create(const glm::vec3& c, const glm::vec3& e);
};

struct RPoint : Point, PolyRender
{
private:
	RPoint();
	RPoint(const glm::vec3& point);
public:	
	RPoint(const RPoint&) = delete;
	RPoint(RPoint&&) = delete;

	RPoint& operator= (const RPoint&) = delete;
	RPoint& operator= (RPoint&&) = delete;

	~RPoint() override;

	static std::shared_ptr<RPoint> Create();
	static std::shared_ptr<RPoint> Create(const glm::vec3& p);
	
	PolyRender* GetRenderSettings() override;
};
