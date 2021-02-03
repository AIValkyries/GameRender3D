#pragma once
#ifndef __Plane2D_H__
#define __Plane2D_H__

#include "DataType.h"
#include "lMathf.h"

class Plane2D
{
public:
	Plane2D();
	Plane2D(Vector2 _normal, float32 _distance) :
		normal(_normal),distance(_distance) {}
	~Plane2D();

	Plane2D& operator=(const Plane2D& plane);
	bool operator==(const Plane2D& plane);
	bool operator!=(const Plane2D& plane);

	Vector2 normal;
	float32 distance;

	void Set(Vector2 _normal,float32 _distance);
	void Set(Vector2 _normal, Vector2 _point);
	void Normalize();

	// 点距离平面的有符号距离
	float32 SignedDistance(const Vector2& point)const;
};

inline Plane2D::Plane2D()
{
	normal == Vector2::zero();
	distance = 0;
}

inline Plane2D::~Plane2D() {}

#pragma region operator

inline Plane2D& Plane2D::operator=(const Plane2D& plane)
{
	normal = plane.normal;
	distance = plane.distance;
	return *this;
}

inline bool Plane2D::operator==(const Plane2D& plane)
{
	return normal == plane.normal && distance == plane.distance;
}

inline bool Plane2D::operator!=(const Plane2D& plane)
{
	return normal != plane.normal || distance != plane.distance;
}

inline bool operator==(const Plane2D& p1, const Plane2D& p2)
{
	return p1.normal == p2.normal && p1.distance == p2.distance;
}

inline bool operator!=(const Plane2D& p1, const Plane2D& p2)
{
	return p1.normal != p2.normal || p1.distance != p2.distance;
}

#pragma endregion

inline void Plane2D::Set(Vector2 _normal, float32 _distance)
{
	normal = _normal;
	normal.Normalize();
	distance = _distance;
}

inline void Plane2D::Set(Vector2 _normal, Vector2 _point)
{
	normal = _normal;
	normal.Normalize();
	distance = -Dot(_normal, _point);
}

// 点距离平面的有符号距离
inline float32 Plane2D::SignedDistance(const Vector2& point)const
{
	return Dot(normal, point) + distance;
}

inline void Plane2D::Normalize()
{
	float32 mag = normal.Magnitude();
	float32 inv_mag = 1.0f / mag;

	normal.x *= inv_mag;
	normal.y *= inv_mag;
	distance *= inv_mag;
}

#endif

