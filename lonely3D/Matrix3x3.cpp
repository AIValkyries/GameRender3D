#include "Matrix3x3.h"
#include "Matrix4x4.h"
#include "Vector3.h"

Matrix3x3 CreateIdentityMatrix3x3f()
{
	Matrix3x3 temp;
	temp.SetIdentity();
	return temp;
}

Matrix3x3 CreateZeroMatrix3x3f()
{
	Matrix3x3 temp;
	temp.SetZero();
	return temp;
}

const Matrix3x3 Matrix3x3::identity = CreateIdentityMatrix3x3f();
const Matrix3x3 Matrix3x3::zero = CreateZeroMatrix3x3f();

#pragma region operator

Matrix3x3& Matrix3x3::operator=(const Matrix3x3& intMat)
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		data[i] = intMat.data[i];
	}

	return *this;
}

Matrix3x3& Matrix3x3::operator=(const Matrix4x4& intMat)
{
	data[0] = intMat[0];
	data[1] = intMat[1];
	data[2] = intMat[2];

	data[3] = intMat[4];
	data[4] = intMat[5];
	data[5] = intMat[6];

	data[6] = intMat[8];
	data[7] = intMat[9];
	data[8] = intMat[10];

	return *this;
}

Matrix3x3& Matrix3x3::operator+=(const Matrix3x3& inMat)
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		data[i] += inMat[i];
	}
	return *this;
}

Matrix3x3& Matrix3x3::operator-=(const Matrix3x3& inMat)
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		data[i] -= inMat[i];
	}
	return *this;
}

Matrix3x3& Matrix3x3::operator*=(const float32 s)
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		data[i] *= s;
	}
	return *this;
}

Vector3 Matrix3x3::operator*=(const Vector3& inVec)
{
	Vector3 tempV(0, 0, 0);
	tempV.x = data[0] * inVec.x + data[1] * inVec.y + data[2] * inVec.z;
	tempV.y = data[3] * inVec.x + data[4] * inVec.y + data[5] * inVec.z;
	tempV.z = data[6] * inVec.x + data[7] * inVec.y + data[8] * inVec.z;
	return tempV;
}

Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& inMat)
{
	for (int i = 0; i < 3; i++)
	{
		// get 行
		int32 index = i * 3;
		float32 row[3] = { data[index + 0],data[index + 1],data[index + 2] };
		
		for (int j = 0; j < 3; j++)
		{
			float32 m0 = row[0] * inMat.Get(0, j);
			float32 m1 = row[1] * inMat.Get(1, j);
			float32 m2 = row[2] * inMat.Get(2, j);
			data[index + j] = m0 + m1 + m2;
		}
	}

	return *this;
}

Matrix3x3& Matrix3x3::operator*=(const Matrix4x4& inMat)
{
	for (int i = 0; i < 3; i++)
	{
		// get 行
		int32 index = i * 3;
		float32 row[3] = { data[index + 0],data[index + 1],data[index + 2] };

		for (int j = 0; j < 3; j++)
		{
			float32 m0 = row[0] * inMat.Get(0, j);
			float32 m1 = row[1] * inMat.Get(1, j);
			float32 m2 = row[2] * inMat.Get(2, j);
			data[index + j] = m0 + m1 + m2;
		}
	}

	return *this;
}

Matrix3x3& Matrix3x3::operator/=(const float32 s)
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		data[i] /= s;
	}

	return *this;
}

Matrix3x3 Matrix3x3::operator-()
{
	Matrix3x3 temp;
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		temp[i] = -data[i];
	}
	return temp;
}

bool Matrix3x3::operator==(const Matrix3x3& inMat)
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		if (data[i] != inMat[i])
			return false;
	}
	return true;
}

bool Matrix3x3::operator!=(const Matrix3x3& inMat)
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		if (data[i] != inMat[i])
			return true;
	}
	return false;
}


#pragma endregion

void Matrix3x3::SetZero()
{
	for (int i = 0; i < DATA_COUNT_3X3; i++)
	{
		data[i] = 0;
	}
}

void Matrix3x3::SetIdentity()
{
	data[0] = 1;
	data[1] = 0;
	data[2] = 0;

	data[3] = 0;
	data[4] = 1;
	data[5] = 0;

	data[6] = 0;
	data[7] = 0;
	data[8] = 1;
}

void Matrix3x3::Scale(const Vector3& scale)
{
	data[0] *= scale.x;
	data[1] *= scale.x;
	data[2] *= scale.x;

	data[3] *= scale.y;
	data[4] *= scale.y;
	data[5] *= scale.y;

	data[6] *= scale.z;
	data[7] *= scale.z;
	data[8] *= scale.z;
}

float32 Matrix3x3::Determinant()
{
	float32 fCofactor0 = data[0] * data[4] * data[8];
	float32 fCofactor1 = data[1] * data[5] * data[6];
	float32 fCofactor2 = data[2] * data[3] * data[7];

	float32 fCofactor3 = data[0] * data[5] * data[7];
	float32 fCofactor4 = data[1] * data[3] * data[8];
	float32 fCofactor5 = data[2] * data[4] * data[6];

	return fCofactor0 + fCofactor1 + fCofactor2 - fCofactor3 - fCofactor4 - fCofactor5;
}

// 转置
Matrix3x3& Matrix3x3::Transpose()
{
	Swap(data[1], data[3]);
	Swap(data[2], data[6]);
	Swap(data[5], data[7]);

	return *this;
}

Matrix3x3 Matrix3x3::Inverse()
{
	Matrix3x3 temp;

	float32 f = 1.0F / Determinant();

	temp[0] = (data[4] * data[5] - data[7] * data[8]);
	temp[1] = -(data[3] * data[5] - data[6] * data[8]);
	temp[2] = (data[3] * data[4] - data[6] * data[7]);
	temp[3] = -(data[1] * data[2] - data[7] * data[8]);
	temp[4] = (data[0] * data[2] - data[6] * data[8]);
	temp[5] = -(data[0] * data[1] - data[6] * data[7]);
	temp[6] = (data[1] * data[2] - data[4] * data[5]);
	temp[7] = -(data[0] * data[2] - data[3] * data[5]);
	temp[8] = (data[0] * data[1] - data[3] * data[4]);

	temp.Transpose();
	temp *= f;

	return temp;
}

// 行
Vector3 Matrix3x3::GetRow(int32 row)
{
	Vector3 temp;

	int32 index = row * 3;
	temp.x = data[index];
	temp.y = data[index + 1];
	temp.z = data[index + 2];

	return temp;
}

// 列
Vector3 Matrix3x3::GetColumn(int32 column)
{
	Vector3 temp;

	temp.x = Get(0, column);
	temp.y = Get(1, column);
	temp.z = Get(2, column);

	return temp;
}

Vector3 Matrix3x3::MultiplyVector3(const Vector3& inVec)const
{
	Vector3 tempV(0, 0, 0);
	tempV.x = data[0] * inVec.x + data[1] * inVec.y + data[2] * inVec.z;
	tempV.y = data[3] * inVec.x + data[4] * inVec.y + data[5] * inVec.z;
	tempV.z = data[6] * inVec.x + data[7] * inVec.y + data[8] * inVec.z;
	return tempV;
}

// ------------------------ 左手坐标系----------------------------//

// 绕任意轴旋转
void BuildAxisAngle(
	Vector3& rotationAxis, 
	const float32& rad, Matrix3x3& mat)
{
	rotationAxis.Normalize();

	float32 c = Cos(float32(rad * DEG_TO_RAD));
	float32 s = Sin(float32(rad * DEG_TO_RAD));
	float32 one_cos = 1 - c;

	float32 xx = rotationAxis.x * rotationAxis.x;
	float32 yy = rotationAxis.y * rotationAxis.y;
	float32 zz = rotationAxis.z * rotationAxis.z;

	float32 xz = rotationAxis.x * rotationAxis.z;
	float32 xy = rotationAxis.x * rotationAxis.y;
	float32 yz = rotationAxis.y * rotationAxis.z;

	float32 zs = rotationAxis.z * s;
	float32 xs = rotationAxis.x * s;
	float32 ys = rotationAxis.y * s;

	mat[0] = xx * one_cos + c;
	mat[1] = xy * one_cos - zs;
	mat[2] = xz * one_cos + ys;

	mat[3] = xy * one_cos + zs;
	mat[4] = yy * one_cos + c;
	mat[5] = yz * one_cos - xs;

	mat[6] = xz * one_cos - ys;
	mat[7] = yz * one_cos + xs;
	mat[8] = zz * one_cos + c;
}

void BuildXAngle(const float32& rad, Matrix4x4& outMatrx)
{
	float32 c = Cos(float32(rad * DEG_TO_RAD));
	float32 s = Sin(float32(rad * DEG_TO_RAD));

	outMatrx[0] = 1;
	outMatrx[1] = 0;
	outMatrx[2] = 0;

	outMatrx[3] = 0;
	outMatrx[4] = c;
	outMatrx[5] = s;

	outMatrx[6] = 0;
	outMatrx[7] = -s;
	outMatrx[8] = c;

}

// 按列摆放???
void BuildYAngle(const float32& rad, Matrix4x4& outMatrx)
{
	float32 s = Sin(float32(rad * DEG_TO_RAD));
	float32 c = Cos(float32(rad * DEG_TO_RAD));
 
	outMatrx[0] = c;
	outMatrx[1] = 0;
	outMatrx[2] = s;

	outMatrx[3] = 0;
	outMatrx[4] = 1;
	outMatrx[5] = 0;

	outMatrx[6] = -s;
	outMatrx[7] = 1;
	outMatrx[8] = c;

}

void BuildZAngle(const float32& rad, Matrix4x4& outMatrx)
{
	float32 s = Sin(float32(rad * DEG_TO_RAD));
	float32 c = Sin(float32(rad * DEG_TO_RAD));

	outMatrx[0] = c;
	outMatrx[1] = s;
	outMatrx[2] = 0;

	outMatrx[3] = -s;
	outMatrx[4] = c;
	outMatrx[5] = 0;

	outMatrx[6] = 0;
	outMatrx[7] = 0;
	outMatrx[8] = 1;
 
}

//-----------------------------表达式转换---------------//
extern void EulerToMatrix3x3(Vector3& euler, Matrix3x3& outMat)
{
	float32 x = float32(euler.x * DEG_TO_RAD);
	float32 y = float32(euler.y * DEG_TO_RAD);
	float32 z = float32(euler.z * DEG_TO_RAD);

	float32 cp = Cos(x);
	float32 sp = Sin(x);

	float32 ch = Cos(y);
	float32 sh = Sin(y);

	float32 cb = Cos(z);
	float32 sb = Sin(z);

	outMat[0] = ch * cb + sh * sp * sb;
	outMat[1] = -ch * sb + sh * sp * cb;
	outMat[2] = sh * cp;

	outMat[3] = sb * cp;
	outMat[4] = cb * cp;
	outMat[5] = -sp;

	outMat[6] = -sh * cb + ch * sp * sb;
	outMat[7] = sb * sh + ch * sp * cb;
	outMat[8] = ch * cp;

}

extern void MatrixToEuler3( Matrix3x3& inMat, Vector3& outV)
{

	if (inMat[6] > 0.99999F)  // 万向节锁
	{
		outV.x = float32(PI * -0.5F);
		outV.y = ATan2(inMat[2], inMat[8]);
		outV.z = 0;
	}
	else if (inMat[6] < -0.999999F)  // 万向节锁
	{
		outV.x = float32(PI * 0.5F);
		outV.y = ATan2(inMat[2], inMat[8]);
		outV.z = 0;
	}
	else
	{
		outV.x = ASin(-inMat[6]);
		outV.y = ATan2(inMat[2], inMat[8]);
		outV.z = ATan2(inMat[4], inMat[5]);
	}
}



