#pragma once
#ifndef __Matrix3x3_H__
#define __Matrix3x3_H__

#include "DataType.h"
#include "Vector3.h"

#define DATA_COUNT_3X3 9

class Vector3;
class Vector4;
class Matrix4x4;

/// <summary>
/// 列向量并且右乘矩阵，因此矩阵的列为坐标轴
/// </summary>
class Matrix3x3
{
public:
	Matrix3x3();
	Matrix3x3(
		float32 m00, float32 m01, float32 m02,
		float32 m10, float32 m11, float32 m12,
		float32 m20, float32 m21, float32 m22)
	{
		data[0] = m00;data[1] = m01;data[2] = m02;
		data[3] = m10;data[4] = m11;data[5] = m12;
		data[6] = m20;data[7] = m21;data[8] = m22;
	}
	~Matrix3x3();

	float32& operator[] (int32 index) { if (index < DATA_COUNT_3X3)return data[index]; else return data[0]; }
	const float32 operator[] (int32 index)const 
	{ 
		if (index < DATA_COUNT_3X3)
			return data[index]; 
		else 
			return 0;
	}
	float32& Get(int32 x, int32 y) { return data[x * 3 + y]; }
	const float32& Get(int32 x, int32 y)const { return data[x * 3 + y]; }
	void Set(int32& index, float32& value)
	{
		if (index < DATA_COUNT_3X3) data[index] = value;
	}

	Matrix3x3& operator = (const Matrix3x3& intMat);
	Matrix3x3& operator = (const Matrix4x4& intMat);
	Matrix3x3& operator += (const Matrix3x3& inMat);
	Matrix3x3& operator -= (const Matrix3x3& inMat);
	Matrix3x3& operator *= (const float32 s);
	Vector3 operator *= (const Vector3& inVec);
	Matrix3x3& operator *= (const Matrix3x3& inMat);
	Matrix3x3& operator *= (const Matrix4x4& intMat);

	Matrix3x3& operator /= (const float32 s);
	Matrix3x3 operator -();
	bool operator ==(const Matrix3x3& inMat);
	bool operator !=(const Matrix3x3& inMat);

	void SetIdentity();
	void SetZero();
	void Scale(const Vector3& scale);
	float32 Determinant();
	Matrix3x3& Transpose();
	Matrix3x3 Inverse();

	Vector3 GetRow(int32 row);
	Vector3 GetColumn(int32 column);

	Vector3 MultiplyVector3(const Vector3& inVec)const;

	static const Matrix3x3 zero;
	static const Matrix3x3 identity;

private:
	float32 data[DATA_COUNT_3X3];
};

inline Matrix3x3::Matrix3x3()
{
	for (int i = 0;i < DATA_COUNT_3X3;i++)
	{
		data[i] = 0;
	}
}
inline Matrix3x3::~Matrix3x3() {}

inline Matrix3x3 operator +(const Matrix3x3& lhs, const Matrix3x3& rhs)
{
	Matrix3x3 temp;
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		temp[i] = lhs[i] + rhs[i];
	}
	return temp;
}

inline Matrix3x3 operator -(const Matrix3x3& lhs, const Matrix3x3& rhs)
{
	Matrix3x3 temp;
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		temp[i] = lhs[i] - rhs[i];
	}
	return temp;
}

inline Matrix3x3 operator *(const float32 s, const Matrix3x3& inMat)
{
	Matrix3x3 temp;
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		temp[i] = inMat[i] * s;
	}
	return temp;
}

inline Matrix3x3 operator *(const Matrix3x3& inMat, const float32 s)
{
	Matrix3x3 temp;
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		temp[i] = inMat[i] * s;
	}
	return temp;
}
inline Matrix3x3 operator *(const Matrix3x3& lhs, const Matrix3x3& rhs)
{
	Matrix3x3 temp;

	for (int i = 0; i < 3; i++)
	{
		// get 行
		int32 index = i * 3;
		float32 row[3] = { lhs[index + 0],lhs[index + 1],lhs[index + 2] };

		for (int j = 0; j < 3; j++)
		{
			float32 m0 = row[0] * rhs.Get(0, j);
			float32 m1 = row[1] * rhs.Get(1, j);
			float32 m2 = row[2] * rhs.Get(2, j);
			temp[index + j] = m0 + m1 + m2;
		}
	}

	return temp;
}

inline Matrix3x3 operator /(const Matrix3x3& inMat, const float32 s)
{
	Matrix3x3 temp;
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		temp[i] = inMat[i] / s;
	}
	return temp;
}

inline Matrix3x3 operator -(const Matrix3x3& inMat)
{
	Matrix3x3 temp;
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		temp[i] = -inMat[i];
	}
	return temp;
}

inline bool operator == (const Matrix3x3& lhs, const Matrix3x3& rhs)
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		if (lhs[i] != rhs[i])
			return false;
	}
	return true;
}

inline bool operator != (const Matrix3x3& lhs, const Matrix3x3& rhs)
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		if (lhs[i] == rhs[i])
			return false;
	}
	return true;
}

// ------------------------ 左手坐标系----------------------------//
extern  void BuildAxisAngle(Vector3& rotationAxis, const float32& rad,Matrix3x3& mat);
extern  void BuildXAngle(const float32& rad, Matrix4x4& outMatrx); // 绕x轴旋转矩阵
extern  void BuildYAngle(const float32& rad, Matrix4x4& outMatrx); // 绕y轴旋转矩阵
extern  void BuildZAngle(const float32& rad, Matrix4x4& outMatrx); // 绕z轴旋转矩阵

extern void EulerToMatrix3x3(Vector3& euler, Matrix3x3& outMat);
extern void MatrixToEuler3(Matrix3x3& inMat, Vector3& outV);





#endif