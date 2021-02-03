#pragma once
#ifndef __Matrix4x4_H__
#define __Matrix4x4_H__

#include "DataType.h"


#define DATA_COUNT_4X4 16
class Vector4;
class Vector3;
class Matrix3x3;
class Quaternion;
class AABB;

class Matrix4x4
{
public:
	Matrix4x4();
	Matrix4x4(
		float32 m00,float32 m01,float32 m02,float32 m03,
		float32 m10,float32 m11,float32 m12,float32 m13,
		float32 m20,float32 m21,float32 m22,float32 m23,
		float32 m30,float32 m31,float32 m32,float32 m33)
	{
		data[0] = m00;data[1] = m01;data[2] = m02;data[3] = m03;
		data[4] = m10;data[5] = m11;data[6] = m12;data[7] = m13;
		data[8] = m20;data[9] = m21;data[10] = m22;data[11] = m23;
		data[12] = m30;data[13] = m31;data[14] = m32;data[15] = m33;
	}
	~Matrix4x4();

	float32& operator[] (int32 index)
	{
		if (index >= 0 && index < DATA_COUNT_4X4)
			return data[index];
		else
			return data[0];
	}
	const float32 operator[] (int32 index)const
	{
		if (index >= 0 && index < DATA_COUNT_4X4)
			return data[index];
		else
			return 0;
	}

	float32 Get(int32 x, int32 y) 
	{ 
		return data[x * 4 + y];
	}
	const float32 Get(int32 x, int32 y)const { return data[x * 4 + y]; }

	Matrix4x4& operator = (const Matrix4x4& inMat);
	Matrix4x4& operator = (const Matrix3x3& inMat);
	Matrix4x4& operator+=(const Matrix4x4& inMat);
	Matrix4x4& operator-=(const Matrix4x4& inMat);
	Matrix4x4& operator*=(const float32 s);
	Vector4 operator*=(const Vector4& inV);
	Vector3 operator*=(const Vector3& inV);
	Matrix4x4& operator*=(const Matrix4x4& inMat);
	Matrix4x4& operator/=(const float32 s);
	Matrix4x4 operator-();
	bool operator==(const Matrix4x4& inMat);
	bool operator!=(const Matrix4x4& inMat);

	void SetIdentity();
	void SetZero();
	float32 Determinant();   // 行列式
	Matrix4x4& Transpose();  // 转置
	Matrix4x4 Inverse();    // 逆

	Vector4 GetColumn(int32 column);
	Vector4 GetRow(int32 row);
	Vector3 GetPositionV3()const;
	Vector4 GetPositionV4()const;
	Vector3 GetScale()const;
	Vector3 GetRotate()const;

	void SetPosition(const Vector4& position);
	void SetPosition(const Vector3& position);
	void SetScale(Vector3& scale);
	void SetScale(float32& x, float32& y, float32& z);
	void SetScale(const float32& x,const float32& y,const float32& z);
	void SetRotate(const Quaternion& q);
	void SetRotate(const Vector3& euler);

	void SetTRS(const Vector3& pos, const Quaternion& q, const Vector3& scale);
	void SetTRS(const Vector4& pos, const Quaternion& q, const Vector3& scale);
	void SetTR(const Vector4& pos, const Quaternion& q);

	Vector3 MultiplyVector3(const Vector3& vector);
	Vector4 MultiplyVector4(const Vector4& vector);

	static const Matrix4x4 zero;
	static const Matrix4x4 identity;

private:
	float32 data[DATA_COUNT_4X4];

};

inline Matrix4x4::Matrix4x4()
{
 	for (int i = 0;i < DATA_COUNT_4X4;i++)
	{
		data[i] = 0;
	}
}
inline Matrix4x4::~Matrix4x4() {}


inline Matrix4x4 operator +(const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	Matrix4x4 temp;

	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		temp[i] = lhs[i] + rhs[i];
	}
	return temp;
}
inline Matrix4x4 operator -(const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	Matrix4x4 temp;

	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		temp[i] = lhs[i] - rhs[i];
	}
	return temp;
}

inline Matrix4x4 operator *(const Matrix4x4& lhs, float32 s)
{
	Matrix4x4 temp;
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		temp[i] = lhs[i] * s;
	}
	return temp;
}

inline Matrix4x4 operator *(const float32 s, const Matrix4x4& rhs)
{
	Matrix4x4 temp;
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		temp[i] = rhs[i] * s;
	}
	return temp;
}

inline static void MultiplyMatrix(Matrix4x4& out,const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	for (int32 x = 0; x < 4; x++)
	{
		int32 xIndex = x * 4;
		float32 rows[4] =
		{
			lhs[xIndex],
			lhs[xIndex + 1],
			lhs[xIndex + 2],
			lhs[xIndex + 3]
		};

		for (int32 y = 0; y < 4; y++)
		{
			float32 m1 = rows[0] * rhs.Get(0, y);
			float32 m2 = rows[1] * rhs.Get(1, y);
			float32 m3 = rows[2] * rhs.Get(2, y);
			float32 m4 = rows[3] * rhs.Get(3, y);
			out[xIndex + y] = m1 + m2 + m3 + m4;
		}
	}
}

 
inline Matrix4x4 operator /(const Matrix4x4& lhs, const float32 s)
{
	Matrix4x4 temp;
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		temp[i] = lhs[i] / s;
	}
	return temp;
}

inline Matrix4x4 operator -(const Matrix4x4& inMat)
{
	Matrix4x4 temp;
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		temp[i] = -inMat[i];
	}
	return temp;
}

inline  bool operator == (const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		if (lhs[i] != rhs[i])
			return false;
	}
	return true;
}

inline  bool operator != (const Matrix4x4& lhs, const Matrix4x4& rhs)
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		if (lhs[i] == rhs[i])
			return false;
	}
	return true;
}

//-----------------------投影矩阵-------------------//
// 正交投影
extern Matrix4x4 MatrixOrthoLH(
	float32 left, float32 right,
	float32 top, float32 bottom,
	float32 zNear, float32 zFar);
//透视投影
extern Matrix4x4 MatrixPerspectiveFovLH(
	float32 fov, float32 aspect, float32 zNear, float32 zFar);

//----------------------角度表达式转换——-----------------//
extern  void EulerToMatrix(const Vector4& inV, Matrix4x4& outMat);
extern  void MatrixToEuler(const Matrix4x4& inMat, Vector4& outV);

extern void TransformBox(Matrix4x4& mat, AABB& box);
extern void TransformBoxEx(Matrix4x4& mat, AABB& box);

#endif
