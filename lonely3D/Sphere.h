#pragma once
#ifndef __Sphere_H__
#define __Sphere_H__

#include "lMathf.h"
#include "Plane3D.h"

class Sphere
{
public:
	Sphere();
	Sphere(Vector3 _center, float32 _radius) :
		center(_center), radius(_radius) {}
	~Sphere();

	Vector3 center;
	float32 radius;

	Sphere& operator=(const Sphere& s);
	bool operator ==(const Sphere& s);
	bool operator !=(const Sphere& s);

	void Set(const Vector3& c, float32 r);

	void Offset(const Vector3& offset);  // ���ĵ�ƫ��
	void Expand(float n);                 // ����뾶
};

inline Sphere::Sphere() 
{
	center = Vector3::zero();
	radius = 0;
}
inline Sphere::~Sphere() {}

inline bool operator == (const Sphere& s1, const Sphere& s2)
{
	return s1.center == s2.center && s1.radius == s2.radius;
}

inline bool operator !=(const Sphere& s1, const Sphere& s2)
{
	return s1.center != s2.center || s1.radius != s2.radius;
}

inline Sphere& Sphere::operator=(const Sphere& s)
{
	center = s.center;radius = s.radius;
	return *this;
}

inline bool Sphere::operator==(const Sphere& s)
{
	return center == s.center && radius == s.radius;
}
inline bool Sphere::operator!=(const Sphere& s)
{
	return center != s.center || radius != s.radius;
}

inline void Sphere::Set(const Vector3& c, float32 r)
{
	center = c;
	radius = r;
}

// ���ĵ�ƫ��
inline void Sphere::Offset(const Vector3& offset)
{
	center += offset;
}

// ����뾶
inline void Sphere::Expand(float n)
{
	radius += n;
}


#endif

