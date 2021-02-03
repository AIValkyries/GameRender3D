#pragma once
#ifndef __Vector2_H__
#define __Vector2_H__

#include "MathTools.h"


class Vector2
{
public:
	Vector2();
	Vector2(float32 _x, float32 _y) :x(_x), y(_y) {}
	~Vector2();

	float32 x;
	float32 y;

	float32& operator[](int32 i) { if (i == 0)return x; else if (i == 1)return y; }
	const float32& operator[](int32 i)const { if (i == 0)return x; else if (i == 1)return y; }

	Vector2& operator +=(const Vector2& inVec) { x += inVec.x; y += inVec.y; return *this; }
	Vector2& operator -=(const Vector2& inVec) { x -= inVec.x; y -= inVec.y;  return *this; }
	Vector2& operator *=(const float32 s) { x *= s; y *= s;  return *this; }
	Vector2& operator /=(const float32 s) { x /= s; y /= s; return *this; }
	Vector2 operator -()const { return Vector2(-x, -y); }

	bool operator==(const Vector2& inVec) { return (inVec.x == x && inVec.y == y); }
	bool operator!=(const Vector2& inVec) { return (inVec.x != x || inVec.y != y); }

	float32 Dot(Vector2 inVec);
	float32 Magnitude();
	float32 SqrMagnitude();
	void Normalize();
	void Set(float32 newX, float32 newY);
	float32 Angle(Vector2 inVec);
	float32 Distance(Vector2 inVec);

	static const Vector2 zero() { return Vector2(0, 0); }
	static const Vector2 one() { return Vector2(1, 1); }
};

inline Vector2::Vector2()
{
	x = 0;y = 0;
}
inline Vector2::~Vector2() {}

inline Vector2 operator+(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x + b.x, a.y + b.y);
}

inline Vector2 operator - (const Vector2& a)
{
	return Vector2(-a.x, -a.y);
}

inline Vector2 operator -(const Vector2& a, const  Vector2& b)
{
	return Vector2(a.x - b.x, a.y - b.y);
}

inline Vector2 operator * (float32 d, const Vector2& a)
{
	return Vector2(a.x * d, a.y * d);
}

inline Vector2 operator *(const Vector2& a, float32 d)
{
	return Vector2(a.x * d, a.y * d);
}

inline Vector2 operator /(const Vector2& a, const float32 t)
{
	return Vector2(a.x / t, a.y / t);
}

inline Vector2 operator /(const Vector2& a, const Vector2& b)
{
	return Vector2(a.x / b.x, a.y / b.y);
}

inline bool operator ==(const Vector2& a, const Vector2& b)
{
	return a.x == b.x && a.y == b.y;
}

inline bool operator !=(const Vector2& a, const Vector2& b)
{
	return a.x != b.x || a.y != b.y;
}

inline bool operator < (const Vector2& a, const Vector2& b)
{
	return a.x < b.x&& a.y < b.y;
}

inline bool operator > (const Vector2& a, const Vector2& b)
{
	return a.x > b.x&& a.y > b.y;
}

inline float32 Angle(Vector2 a, Vector2 b)
{
	return a.Angle(b);
}

inline float32 Distance(Vector2 a, Vector2 b)
{
	return a.Distance(b);
}

inline float32 Dot(Vector2 a, Vector2 b)
{
	return a.x * b.x + a.y * b.y;
}

inline Vector2 Lerp(Vector2 form, Vector2 to, float32 t)
{
	return (to - form) * t + form;
}

inline Vector2 Max(Vector2 lhs, Vector2 rhs)
{
	return Vector2(Max(lhs.x, rhs.x), Max(lhs.y, rhs.y));
}

inline Vector2 Min(Vector2 lhs, Vector2 rhs)
{
	return Vector2(Min(lhs.x, rhs.x), Min(lhs.y, rhs.y));
}

inline Vector2 Reflect(Vector2 inDirection, Vector2 inNormal)
{
	return inDirection-(2 * (Dot(inDirection, inNormal)) * inNormal);
}

inline Vector2 Project(Vector2 vector, Vector2 onNormal)
{
	onNormal.Normalize();
	return Dot(vector, onNormal) * onNormal;
}



class Vector2D
{
public:
	Vector2D()
	{
		x = 0;y = 0;
	}
	Vector2D(float64 _x, float64 _y) :x(_x), y(_y) {}

	Vector2D& operator *=(const float64 s)
	{
		x *= s;y *= s;
		return *this;
	}

	Vector2D& operator /=(const float64 s)
	{
		x /= s;
		y /= s;
		return *this;
	}

	Vector2D& operator=(const Vector2& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	float64 x;
	float64 y;

	void Set(float64 _x, float64 _y)
	{
		x = _x;
		y = _y;
	}
};



#endif
