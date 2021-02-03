#pragma once
#ifndef __Plane3D_H__
#define __Plane3D_H__

#include "DataType.h"
#include "lMathf.h"
#include "Debug.h"

class Plane3D
{
public:
	Plane3D();
	Plane3D(Vector3 _normal, float32 _distance) 
		:normal(_normal),distance(_distance) {}
	~Plane3D();

	Plane3D& operator=(const Plane3D& plane);
	bool operator == (const Plane3D& plane);
	bool operator !=(const Plane3D& plane);

	Vector3 normal;
	float32 distance;

	void Set(Vector3 point1, Vector3 point2, Vector3 point3);
	void Set(Vector3 _normal,float32 _distance);
	void Set(Vector3 _normal,Vector3 _point1);

	float32 SignedDistance(const Vector3& point)const;

	void Normalize();

	string ToString()const;

};

inline Plane3D::Plane3D() 
{
	normal = Vector3::zero();
	distance = 0;
}
inline Plane3D::~Plane3D() {}

#pragma region operator

inline Plane3D& Plane3D::operator=(const Plane3D& plane)
{
	normal = plane.normal;
	distance = plane.distance;

	return *this;
}

inline bool Plane3D::operator==(const Plane3D& plane)
{
	return normal == plane.normal && distance == plane.distance;
}

inline bool Plane3D::operator!=(const Plane3D& plane)
{
	return normal != plane.normal || distance != plane.distance;
}

inline bool operator == (const Plane3D& p1,const Plane3D& p2)
{
	return p1.normal == p2.normal && p1.distance == p2.distance;
}

inline bool operator != (const Plane3D& p1, const Plane3D& p2)
{
	return p1.normal != p2.normal || p1.distance != p2.distance;
}

#pragma endregion

inline void Plane3D::Set(Vector3 point1, Vector3 point2, Vector3 point3)
{
	Vector3 SideA(point2 - point1);
	Vector3 SideB(point3 - point1);

	normal = Cross(SideB, SideA);
	normal.Normalize();

	distance = -Dot(normal, point1);
}

inline void Plane3D::Set(Vector3 _normal, float32 _distance)
{
	normal = _normal;
	distance = _distance;
}

inline void Plane3D::Set(Vector3 _normal, Vector3 _point1)
{
	_normal.Normalize();
	normal = _normal;
	distance = -Dot(normal, _point1);
}

inline float32 Plane3D::SignedDistance(const Vector3& point)const
{
	return Dot(normal, point) + distance;
}

inline void Plane3D::Normalize()
{
	float32 mag = normal.Magnitude();

	if (mag > 0.0F)
	{
		float32 inv_mag = 1.0F / mag;

		normal.x *= inv_mag;
		normal.y *= inv_mag;
		normal.z *= inv_mag;
		distance *= inv_mag;
	}
}


inline string Plane3D::ToString()const
{
	string logStr;

	logStr = "Normal:(" + Convert(normal.x) + "," + Convert(normal.y) + ","
		+ Convert(normal.z) + "),Distance:" + Convert(distance);
	
	return logStr;
}

#endif
