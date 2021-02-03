#pragma once
#ifndef __Vector3_H__
#define __Vector3_H__

#include "MathTools.h"


class Vector2;
class Vector4;
class Vector3
{
public:
	Vector3();
	Vector3(float32 _x, float32 _y, float32 _z) :x(_x), y(_y), z(_z) {}
	Vector3(const Vector2& inVec);
	Vector3(const Vector3& inVec);
	Vector3(const Vector4& inVec);
	~Vector3();

	float32 x, y, z;

	Vector3& operator() (Vector2 inVec);
	Vector3& operator() (Vector4 inVec);

	Vector3& operator =(Vector2 inVec);
	Vector3& operator =(Vector4 inVec);

	//Vector3& operator=(const Vector2& inVec);
	//Vector3& operator=(const Vector4& inVec);

	float32 operator[] (int32 index) { if (index == 0)return x; else if (index == 1) return y; else if (index == 2) return z; }
	const float32 operator[] (int32 index)const { if (index == 0)return x; else if (index == 1) return y; else if (index == 2) return z; }

	Vector3& operator +=(const Vector3 inVec) { x += inVec.x; y += inVec.y; z += inVec.z; return *this; }
	Vector3& operator -=(const Vector3 inVec) { x -= inVec.x; y -= inVec.y; z -= inVec.z; return *this; }
	Vector3& operator *=(const float32 d) { x *= d; y *= d; z *= d; return *this; }
	Vector3& operator /=(const float32 d) { x /= d; y /= d; z /= d; return *this; }
	Vector3 operator -() { return Vector3(-x, -y, -z); }

	bool operator == (Vector3 inVec) { return x == inVec.x && y == inVec.y && z == inVec.z; }
	bool operator !=(Vector3 inVec) { return x != inVec.x || y != inVec.y || z != inVec.z; }

	float32 Dot(const Vector3& inVec);
	float32 Dot(const Vector4& inVec);
	float32 Magnitude();
	float32 SqrMagnitude();
	void Normalize();
	void Set(float32 newX, float32 newY, float32 newZ);
	void Set(const Vector4& inVec);
	void AddEqual(const Vector4& inVec);
	void AddEqual(const Vector3& inVec);
	void  Clear() { x = 0; y = 0; z = 0;}

	static const Vector3 zero() { return Vector3(0, 0, 0); }
	static const Vector3 one() { return Vector3(1, 1, 1); }
	static const Vector3 forward() { return Vector3(0, 0, 1); }
	static const Vector3 back() { return  Vector3(0, 0, -1); }
	static const Vector3 left() { return Vector3(-1, 0, 0); }
	static const Vector3 right() { return Vector3(1, 0, 0); }
	static const Vector3 up() { return Vector3(0, 1, 0); }
	static const Vector3 down() { return  Vector3(0, -1, 0); }
};

inline Vector3::Vector3()
{
	x = 0;y = 0;z = 0;
}
inline Vector3::~Vector3() {}

inline Vector3 operator +(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x + b.x, a.y + b.y, a.z + b.z);
}

inline Vector3 operator -(const Vector3& a, const Vector3& b)
{
	return Vector3(a.x - b.x, a.y - b.y, a.z - b.z);
}

inline Vector3 operator * (float32 t, const Vector3& v)
{
	return Vector3(v.x * t, v.y * t, v.z * t);
}

inline Vector3 operator * (const Vector3& v, float32 t)
{
	return Vector3(v.x * t, v.y * t, v.z * t);
}

inline Vector3 operator /(const Vector3& v, float32 t)
{
	return Vector3(v.x / t, v.y / t, v.z / t);
}

inline bool operator ==(const Vector3& a, const Vector3& b)
{
	return (a.x == b.x && a.y == b.y && a.z == b.z);
}

inline bool operator !=(const Vector3& a, const Vector3& b)
{
	return (a.x != b.x || a.y != b.y || a.z != b.z);
}

inline Vector3 operator -(const Vector3& a)
{
	return Vector3(-a.x, -a.y, -a.z);
}


inline bool operator < (const Vector3& a, const Vector3& b)
{
	return a.x < b.x && a.y < b.y&& a.z < b.z;
}

inline bool operator > (const Vector3& a, const Vector3& b)
{
	return a.x > b.x && a.y > b.y && a.z > b.z;
}

inline  float32 Dot(Vector3 a, Vector3 b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float32 Angle(Vector3 a, Vector3 b)
{
	a.Normalize();
	b.Normalize();

	float32 rad = ACos(Dot(a, b));
	return (float32)(rad * RAD_TO_DEG);
}

inline  float32 Distance(Vector3 a, Vector3 b)
{
	Vector3 dis = a - b;
	return dis.Magnitude();
}

inline Vector3 Cross(Vector3 a, Vector3 b)
{
	float32 x = a.y * b.z - a.z * b.y;
	float32 y = a.z * b.x - a.x * b.z;
	float32 z = a.x * b.y - a.y * b.x;

	return Vector3(x, y, z);
}

inline  Vector3 Lerp(Vector3 form, Vector3 to, float32 t)
{
	return (to - form) * t + form;
}

inline  Vector3 Max(Vector3 lhs, Vector3 rhs)
{
	return Vector3(Max(lhs.x, rhs.x), Max(lhs.y, rhs.y), Max(lhs.z, rhs.z));
}

inline  Vector3 Min(Vector3 lhs, Vector3 rhs)
{
	return Vector3(Min(lhs.x, rhs.x), Min(lhs.y, rhs.y), Min(lhs.z, rhs.z));
}

inline  Vector3 Reflect(Vector3 inDirection, Vector3 inNormal)
{
	return inDirection - 2 * (Dot(inDirection, inNormal) * inNormal);
}

inline  Vector3 Project(Vector3 vector, Vector3 onNormal)
{
	onNormal.Normalize();
	return Dot(vector, onNormal) * onNormal;
}

#endif

