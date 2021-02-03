#pragma once
#ifndef __Quaternion_H__
#define __Quaternion_H__

#include "DataType.h"
#include "MathTools.h"


class Vector3;
class Matrix4x4;
class Matrix3x3;

class Quaternion
{
public:
	Quaternion();
	Quaternion(float32 _x, float32 _y, float32 _z, float32 _w) :x(_x), y(_y), z(_z), w(_w) {}
	Quaternion(float32 _x, float32 _y, float32 _z) :x(_x), y(_y), z(_z), w(1) {}
	~Quaternion();

	float32 x, y, z, w;

	Quaternion& operator+=(const Quaternion& inVec) { x += inVec.x; y += inVec.y; z += inVec.z; w += inVec.w; return *this; }
	Quaternion& operator-=(const Quaternion& inVec) { x -= inVec.x; y -= inVec.y; z -= inVec.z; w -= inVec.w; return *this; }
	Quaternion& operator*=(const float32 s) { x *= s; y *= s; z *= s; w *= s; return *this; }
	Quaternion& operator/=(const float32 s) { x /= s; y /= s; z /= s; w /= s; return *this; }
	Quaternion operator -() 
	{ 
		Quaternion temp(-x, -y, -z, -w);
		return temp;
	}
	Quaternion& operator*=(const Quaternion& inVec)
	{
		float32 xx = w * inVec.x + inVec.w * x + (y * inVec.z - z * inVec.y);
		float32 yy = w * inVec.y + inVec.w * y + (z * inVec.x - x * inVec.z);
		float32 zz = w * inVec.z + inVec.w * z + (x * inVec.y - y * inVec.x);
		float32 ww = w * inVec.w - (inVec.x * x + inVec.y * y + inVec.z * z);
		x = xx; y = yy; z = zz; w = ww;
		return *this;
	}

	bool operator == (const Quaternion& inVec) { return x == inVec.x && y == inVec.y && z == inVec.z && w == inVec.w; }
	bool operator !=(const Quaternion& inVec) { return x != inVec.x || y != inVec.y || z != inVec.z || w != inVec.w; }

	float32 Dot(Quaternion inVec);
	float32 Magnitude();
	float32 SqrMagnitude();

	void Normalize();
	void Set(float32 newX, float32 newY, float32 newZ, float32 newW);
	Quaternion Inverse();   // Äæ
	Quaternion Conjugation(); // ¹²éî

	static Quaternion identity() { return Quaternion(0, 0, 0, 1); }
};

inline Quaternion::Quaternion()
{
	x = 0;y = 0;z = 0;w = 0;
}
inline Quaternion::~Quaternion() {}

inline Quaternion operator +(const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);
}

inline Quaternion operator -(const Quaternion& a, const Quaternion& b)
{
	return Quaternion(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w);
}

inline Quaternion operator *(const float32 s, const Quaternion& a)
{
	return Quaternion(a.x * s, a.y * s, a.z * s, a.w * s);
}

inline Quaternion operator *(const Quaternion& a, const float32 s)
{
	return Quaternion(a.x * s, a.y * s, a.z * s, a.w * s);
}

inline Quaternion operator/(const Quaternion& a, const float32 s)
{
	return Quaternion(a.x / s, a.y / s, a.z / s, a.w / s);
}

inline Quaternion operator*(const Quaternion& lhs, const Quaternion& rhs)
{
	float32 xx = (rhs.w * lhs.x) + (rhs.x * lhs.w) + (rhs.y * lhs.z) - (rhs.z * lhs.y);
	float32 yy = (rhs.w * lhs.y) + (rhs.y * lhs.w) + (rhs.z * lhs.x) - (rhs.x * lhs.z);
	float32 zz = (rhs.w * lhs.z) + (rhs.z * lhs.w) + (rhs.x * lhs.y) - (rhs.y * lhs.x);
	float32 ww = (rhs.w * lhs.w) - (rhs.x * lhs.x) - (rhs.y * lhs.y) - (rhs.z * lhs.z);

	return Quaternion(xx, yy, zz, ww);
}

inline Quaternion operator -(const Quaternion& a)
{
	return Quaternion(-a.x, -a.y, -a.z, -a.w);
}

inline float32 Dot(Quaternion& a, Quaternion& b)
{
	return a.Dot(b);
}

inline Quaternion Lerp(Quaternion& form, Quaternion& to, float32 t)
{
	Quaternion temp ;

	if (Dot(form, to) < 0.0F) //  ¶Û½Ç£¬Ë«±¶¸²¸Ç
	{
		temp = -to;
		temp = (temp - form) * t + form;
		temp.Normalize();
		return temp;
	}
	temp = (to - form) * t + form;
	temp.Normalize();

	return temp;
}

inline Quaternion Slerp(Quaternion& form, Quaternion& to, float32 t)
{
	float32 dot = Dot(form, to);
	Quaternion temp;

	if (dot < 0.0F)  // ¶Û½Ç£¬Ë«±¶¸²¸Ç
	{
		dot = -dot;
		temp.Set(-to.x, -to.y, -to.z, -to.w);
	}
	else
		temp = to;

	if (dot < 0.95F)
	{
		float32 angle = ACos(dot);

		float32 sinadiv = 1.0F / Sin(angle);
		float32 sinat = Sin(t * angle);
		float32 sinaomt = Sin((1 - t) * angle);

		return sinaomt * sinadiv * form + sinat * sinadiv * temp;
	}
	else
	{
		return Lerp(form, to, t);
	}
}



//------------------- ±í´ïÊ½×ª»»---------------------//
extern  void EulerToQuaternion(const Vector3& eulerAngle, Quaternion& outQ);
 extern  void QuaternionToEuler(const Quaternion& inQ, Vector3& outV);

 extern  void MatrixToQuaternion(const Matrix4x4& inMat, Quaternion& outQ);
 extern  void MatrixToQuaternion(const Matrix3x3& inMat, Quaternion& outQ);
 extern  void QuaternionToMatrix(const Quaternion& inQ, Matrix3x3& outMat);
 extern  void QuaternionToMatrix(const Quaternion& inQ, Matrix4x4& outMat);



#endif

