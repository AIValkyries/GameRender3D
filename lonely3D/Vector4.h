#pragma once
#ifndef __Vector4_H__
#define __Vector4_H__

#include "MathTools.h"


class Vector2;
class Vector3;
class Vector4
{
public:
	Vector4();
	Vector4(float32 _x, float32 _y, float32 _z, float32 _w) :x(_x), y(_y), z(_z), w(_w) {}
	Vector4(float32 _x, float32 _y, float32 _z) :x(_x), y(_y), z(_z), w(1) {}
	~Vector4();

	float32 x, y, z, w;

	//Vector4& operator =(Vector2 inVec);
	//Vector4& operator =(Vector3 inVec);
	Vector4& operator =(const Vector2& inVec);
	Vector4& operator =(const Vector3& inVec);

	float32 operator[] (int32 index) { if (index == 0)return x; else if (index == 1)return y; else if (index == 2)return z; else if (index == 3)return w; }
	const float32 operator[] (int32 index)const { if (index == 0)return x; else if (index == 1)return y; else if (index == 2)return z; else if (index == 3)return w; }

	Vector4& operator+=(const Vector4& inVec) { x += inVec.x; y += inVec.y; z += inVec.z;w += inVec.w; return *this; }
	Vector4& operator+=(Vector4& inVec) 
	{ 
		x += inVec.x; 
		y += inVec.y; 
		z += inVec.z;
		w += inVec.w;
		return *this;
	}
	Vector4& operator+=(Vector3& inVec);

	Vector4& operator-=(Vector4& inVec) { x -= inVec.x; y -= inVec.y; z -= inVec.z;w -= inVec.w; return *this; }
	Vector4& operator*=(const float32 s) { x *= s; y *= s; z *= s;w *= s; return *this; }
	Vector4& operator/=(const float32 s) { x /= s; y /= s; z /= s;w /= s; return *this; }
	Vector4 operator- () { return Vector4(-x, -y, -z, -w); }

	bool operator==(const Vector4& inVec) { return x == inVec.x && y == inVec.y && z == inVec.z && w == inVec.w; }
	bool operator!=(const Vector4& inVec) { return x != inVec.x || y != inVec.y || z != inVec.z || w != inVec.w; }

	float32 Dot(const Vector4& inVec);
	float32 Dot(const Vector3& inVec);
	float32 Magnitude();
	float32 SqrMagnitude();
	void Normalize();
	void Abs();
	void Set(float32 newX, float32 newY, float32 newZ, float32 newW);
	void  Clear() { x = 0; y = 0; z = 0; w = 1; }

	static const Vector4 zero() { return Vector4(0, 0, 0); }
	static const Vector4 one() { return Vector4(1, 1, 1); }
	static const Vector4 forward() { return Vector4(0, 0, 1); }
	static const Vector4 back() { return  Vector4(0, 0, -1); }
	static const Vector4 left() { return Vector4(-1, 0, 0); }
	static const Vector4 right() { return Vector4(1, 0, 0); }
	static const Vector4 up() { return Vector4(0, 1, 0); }
	static const Vector4 down() { return  Vector4(0, -1, 0); }

};

inline Vector4::Vector4()
{
	x = 0;y = 0;z = 0;w = 0;
}
inline Vector4::~Vector4() {}

inline Vector4 operator +(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline Vector4 operator-(const Vector4& a, const Vector4& b)
{
	return Vector4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline Vector4 operator*(float32 d, const Vector4& a)
{
	return Vector4(a.x * d, a.y * d, a.z * d, a.w * d);
}

inline Vector4 operator*(const Vector4& a, float32 d)
{
	return Vector4(a.x * d, a.y * d, a.z * d, a.w * d);
}

inline Vector4 operator/(const Vector4& a, float32 d)
{
	return Vector4(a.x / d, a.y / d, a.z / d, a.w / d);
}

inline bool operator == (const Vector4& lhs, const Vector4& rhs)
{
	return (lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w);
}

inline bool operator !=(const Vector4& lhs, const Vector4& rhs)
{
	return lhs.x != rhs.x || lhs.y != rhs.y || lhs.z != rhs.z || lhs.w != rhs.w;
}

inline Vector4 operator-(const Vector4& a)
{
	return Vector4(-a.x, -a.y, -a.z, -a.w);
}


inline float32 Dot(const Vector4& a, const Vector4& b)
{
	float32 xx = a.x * b.x;
	float32 yy = a.y * b.y;
	float32 zz = a.z * b.z;
	float32 ww = a.w * b.w;

	return xx + yy + zz + ww;
}
// ∑µªÿΩ«∂»÷∆
inline float32 Angle(Vector4 a, Vector4 b)
{
	a.Normalize();
	b.Normalize();

	float32 rad = ACos(Dot(a, b));
	return float32(rad * RAD_TO_DEG);
}
inline float32 Distance(Vector4 a, Vector4 b)
{
	Vector4 newV = a - b;
	return newV.Magnitude();
}
inline Vector4 Lerp(Vector4 form, Vector4 to, float32 t)
{
	return(to - form) * t + form;
}

inline Vector4 Max(Vector4 lhs, Vector4 rhs)
{
	return Vector4(Max(lhs.x, rhs.x), Max(lhs.y, rhs.y), Max(lhs.z, rhs.z), Max(lhs.w, rhs.w));
}

inline Vector4 Min(Vector4 lhs, Vector4 rhs)
{
	return Vector4(Min(lhs.x, rhs.x), Min(lhs.y, rhs.y), Min(lhs.z, rhs.z), Min(lhs.w, rhs.w));
}

inline Vector4 Reflect(Vector4 inDirection, Vector4 inNormal)
{
	return	inDirection - Dot(inDirection, inNormal) * inNormal * 2;
}

inline Vector4 Project(Vector4 vector, Vector4 onNormal)
{
	onNormal.Normalize();
	return Dot(vector, onNormal) * onNormal;
}

#endif