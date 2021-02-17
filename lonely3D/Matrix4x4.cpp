#include "Matrix4x4.h"
#include "Vector4.h"
#include "Vector3.h"
#include "Matrix3x3.h"
#include "Quaternion.h"
#include "AABB.h"
#include "Debug.h"

Matrix4x4 CreateIdentityMatrix3x3f()
{
	Matrix4x4 temp;
	temp.SetIdentity();
	return temp;
}

Matrix4x4 CreateZeroMatrix3x3f()
{
	Matrix4x4 temp;
	temp.SetZero();
	return temp;
}

const Matrix4x4 Matrix4x4::identity = CreateIdentityMatrix3x3f();
const Matrix4x4 Matrix4x4::zero = CreateZeroMatrix3x3f();

#pragma region operator

Matrix4x4& Matrix4x4::operator=(const Matrix4x4& inMat)
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		data[i] = inMat[i];
	}
	return *this;
}

Matrix4x4& Matrix4x4::operator=(const Matrix3x3& inMat)
{
	data[0] = inMat[0];
	data[1] = inMat[1];
	data[2] = inMat[2];
	data[3] = 0;

	data[4] = inMat[3];
	data[5] = inMat[4];
	data[6] = inMat[5];
	data[7] = 0;

	data[8] = inMat[6];
	data[9] = inMat[7];
	data[10] = inMat[8];
	data[11] = 0;

	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 1;

	return *this;
}

Matrix4x4& Matrix4x4::operator+=(const Matrix4x4& inMat)
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		data[i] += inMat[i];
	}

	return *this;
}

Matrix4x4& Matrix4x4::operator-=(const Matrix4x4& inMat)
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		data[i] -= inMat[i];
	}
	return *this;
}

Matrix4x4& Matrix4x4::operator*=(const float32 s)
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		data[i] *= s;
	}
	return *this;
}

Vector4 Matrix4x4::operator*=(const Vector4& inV)
{
	Vector4 temp;
	temp.x = inV.x*data[0] + inV.y*data[1] + inV.z*data[2] + inV.w*data[3];
	temp.y = inV.x*data[4] + inV.y*data[5] + inV.z*data[6] + inV.w*data[7];
	temp.z = inV.x*data[8] + inV.y*data[9] + inV.z*data[10] + inV.w*data[11];
	temp.w = inV.x*data[12] + inV.y*data[13] + inV.z*data[14] + inV.w*data[15];
	return temp;
}

Vector3 Matrix4x4::operator*=(const Vector3& inV)
{
	Vector3 temp;
	temp.x = inV.x * data[0] + inV.y * data[1] + inV.z * data[3];
	temp.y = inV.x * data[4] + inV.y * data[5] + inV.z * data[6];
	temp.z = inV.x * data[8] + inV.y * data[9] + inV.z * data[10];
	return temp;
}

Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& inMat)
{
	for (int32 x = 0; x < 4; x++) // 行
	{
		int32 xIndex = x * 4;
		float32 row[4] = { data[xIndex],data[xIndex + 1],data[xIndex + 2],data[xIndex + 3] };

		for (int32 y = 0; y < 4; y++) // 列
		{
			float32 m1 = row[0] * inMat.Get(0, y);
			float32 m2 = row[1] * inMat.Get(1, y);
			float32 m3 = row[2] * inMat.Get(2, y);
			float32 m4 = row[3] * inMat.Get(3, y);
			
			data[xIndex + y] = m1 + m2 + m3 + m4;
		}
	}
	return *this;
}

Matrix4x4& Matrix4x4::operator/=(const float32 s)
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		data[i] /= s;
	}
	return *this;
}

Matrix4x4 Matrix4x4::operator-()
{
	Matrix4x4 temp;
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		temp[i] = -data[i];
	}
	return temp;
}

bool Matrix4x4::operator==(const Matrix4x4& inMat)
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		if (data[i] != inMat[i])
			return false;
	}
	return true;
}

bool Matrix4x4::operator!=(const Matrix4x4& inMat)
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		if (data[i] == inMat[i])
			return false;
	}
	return true;
}

#pragma endregion

void Matrix4x4::SetIdentity()
{
	data[0] = 1;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;

	data[4] = 0;
	data[5] = 1;
	data[6] = 0;
	data[7] = 0;

	data[8] = 0;
	data[9] = 0;
	data[10] = 1;
	data[11] = 0;

	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 1;
}

void Matrix4x4::SetZero()
{
	for (int32 i = 0; i < DATA_COUNT_4X4; i++)
	{
		data[i] = 0;
	}
}

float32 Matrix4x4::Determinant()
{
	float32 m00 = data[0];
	float32 m01 = data[1];
	float32 m02 = data[2];
	float32 m03 = data[3];

	Matrix3x3 m00Mat;
	Matrix3x3 m01Mat;
	Matrix3x3 m02Mat;
	Matrix3x3 m03Mat;

	m00Mat[0]= data[5]; m00Mat[1]= data[6]; m00Mat[2]= data[7];
	m00Mat[3]= data[9]; m00Mat[4]= data[10]; m00Mat[5]= data[11];
	m00Mat[6] = data[13]; m00Mat[7] = data[14]; m00Mat[8] = data[15];

	m01Mat[0] = data[4]; m01Mat[1] = data[6]; m01Mat[2] = data[7];
	m01Mat[3] = data[8]; m01Mat[4] = data[10]; m01Mat[5] = data[11];
	m01Mat[6] = data[12]; m01Mat[7] = data[14]; m01Mat[8] = data[15];

	m02Mat[0] = data[4]; m02Mat[1] = data[5]; m02Mat[2] = data[7];
	m02Mat[3] = data[8]; m02Mat[4] = data[9]; m02Mat[5] = data[11];
	m02Mat[6] = data[12]; m02Mat[7] = data[13]; m02Mat[8] = data[15];

	m03Mat[0] = data[4]; m03Mat[1] = data[5]; m03Mat[2] = data[6];
	m03Mat[3] = data[8]; m03Mat[4] = data[9]; m03Mat[5] = data[10];
	m03Mat[6] = data[12]; m03Mat[7] = data[13]; m03Mat[8] = data[14];

	return m00 * m00Mat.Determinant() - m01 * m01Mat.Determinant()
		+ m02 * m02Mat.Determinant() - m03 * m03Mat.Determinant();
}

// 转置
Matrix4x4& Matrix4x4::Transpose()
{
	Swap(data[1], data[4]);
	Swap(data[2], data[8]);
	Swap(data[3], data[12]);

	Swap(data[6], data[9]);
	Swap(data[7], data[13]);
	Swap(data[11], data[14]);

	return *this;
}

// 逆 伴随矩阵*(1/|A(行列式)|)
Matrix4x4 Matrix4x4::Inverse()
{
	// 由所有元素代数余子式组成的转置矩阵叫伴随矩阵

	//-------------------------------第一行---------------------//
	Matrix3x3 m00Mat;Matrix3x3 m01Mat;
	Matrix3x3 m02Mat;Matrix3x3 m03Mat;

	m00Mat[0] = data[5]; m00Mat[1] = data[6]; m00Mat[2] = data[7];
	m00Mat[3] = data[9]; m00Mat[4] = data[10]; m00Mat[5] = data[11];
	m00Mat[6] = data[13]; m00Mat[7] = data[14]; m00Mat[8] = data[15];

	m01Mat[0] = data[4]; m01Mat[1] = data[6]; m01Mat[2] = data[7];
	m01Mat[3] = data[8]; m01Mat[4] = data[10]; m01Mat[5] = data[11];
	m01Mat[6] = data[12]; m01Mat[7] = data[14]; m01Mat[8] = data[15];

	m02Mat[0] = data[4]; m02Mat[1] = data[5]; m02Mat[2] = data[7];
	m02Mat[3] = data[8]; m02Mat[4] = data[9]; m02Mat[5] = data[11];
	m02Mat[6] = data[12]; m02Mat[7] = data[13]; m02Mat[8] = data[15];

	m03Mat[0] = data[4]; m03Mat[1] = data[5]; m03Mat[2] = data[6];
	m03Mat[3] = data[8]; m03Mat[4] = data[9]; m03Mat[5] = data[10];
	m03Mat[6] = data[12]; m03Mat[7] = data[13]; m03Mat[8] = data[14];

	//-----------------------------第二行---------------------//
	Matrix3x3 m10Mat;Matrix3x3 m11Mat;
	Matrix3x3 m12Mat;Matrix3x3 m13Mat;

	m10Mat[0] = data[1]; m10Mat[1] = data[2]; m10Mat[2] = data[3];
	m10Mat[3] = data[9]; m10Mat[4] = data[10]; m10Mat[5] = data[11];
	m10Mat[6] = data[13]; m10Mat[7] = data[14]; m10Mat[8] = data[15];

	m11Mat[0] = data[0]; m11Mat[1] = data[2]; m11Mat[2] = data[3];
	m11Mat[3] = data[8]; m11Mat[4] = data[10]; m11Mat[5] = data[11];
	m11Mat[6] = data[12]; m11Mat[7] = data[14]; m11Mat[8] = data[15];

	m12Mat[0] = data[0]; m12Mat[1] = data[1]; m12Mat[2] = data[3];
	m12Mat[3] = data[8]; m12Mat[4] = data[9]; m12Mat[5] = data[11];
	m12Mat[6] = data[12]; m12Mat[7] = data[13]; m12Mat[8] = data[15];

	m13Mat[0] = data[0]; m13Mat[1] = data[1]; m13Mat[2] = data[2];
	m13Mat[3] = data[8]; m13Mat[4] = data[9]; m13Mat[5] = data[10];
	m13Mat[6] = data[12]; m13Mat[7] = data[13]; m13Mat[8] = data[14];

	//-----------------------------第三行---------------------//
	Matrix3x3 m20Mat;Matrix3x3 m21Mat;
	Matrix3x3 m22Mat;Matrix3x3 m23Mat;

	m20Mat[0] = data[1]; m20Mat[1] = data[2]; m20Mat[2] = data[3];
	m20Mat[3] = data[5]; m20Mat[4] = data[6]; m20Mat[5] = data[7];
	m20Mat[6] = data[13]; m20Mat[7] = data[14]; m20Mat[8] = data[15];

	m21Mat[0] = data[0]; m21Mat[1] = data[2]; m21Mat[2] = data[3];
	m21Mat[3] = data[4]; m21Mat[4] = data[6]; m21Mat[5] = data[7];
	m21Mat[6] = data[12]; m21Mat[7] = data[14]; m21Mat[8] = data[15];

	m22Mat[0] = data[0]; m22Mat[1] = data[1]; m22Mat[2] = data[3];
	m22Mat[3] = data[4]; m22Mat[4] = data[5]; m22Mat[5] = data[7];
	m22Mat[6] = data[12]; m22Mat[7] = data[13]; m22Mat[8] = data[15];

	m23Mat[0] = data[0]; m23Mat[1] = data[1]; m23Mat[2] = data[2];
	m23Mat[3] = data[4]; m23Mat[4] = data[5]; m23Mat[5] = data[6];
	m23Mat[6] = data[12]; m23Mat[7] = data[13]; m23Mat[8] = data[14];

	//-----------------------------第四行---------------------//
	Matrix3x3 m30Mat;Matrix3x3 m31Mat;
	Matrix3x3 m32Mat;Matrix3x3 m33Mat;

	m30Mat[0] = data[1]; m30Mat[1] = data[2]; m30Mat[2] = data[3];
	m30Mat[3] = data[5]; m30Mat[4] = data[6]; m30Mat[5] = data[7];
	m30Mat[6] = data[9]; m30Mat[7] = data[10]; m30Mat[8] = data[11];

	m31Mat[0] = data[0]; m31Mat[1] = data[2]; m31Mat[2] = data[3];
	m31Mat[3] = data[4]; m31Mat[4] = data[6]; m31Mat[5] = data[7];
	m31Mat[6] = data[8]; m31Mat[7] = data[10]; m31Mat[8] = data[11];

	m32Mat[0] = data[0]; m32Mat[1] = data[1]; m32Mat[2] = data[3];
	m32Mat[3] = data[4]; m32Mat[4] = data[5]; m32Mat[5] = data[7];
	m32Mat[6] = data[8]; m32Mat[7] = data[9]; m32Mat[8] = data[11];

	m33Mat[0] = data[0]; m33Mat[1] = data[1]; m33Mat[2] = data[2];
	m33Mat[3] = data[4]; m33Mat[4] = data[5]; m33Mat[5] = data[6];
	m33Mat[6] = data[8]; m33Mat[7] = data[9]; m33Mat[8] = data[10];

	float32 m00d = m00Mat.Determinant();
	float32 m01d = m01Mat.Determinant();
	float32 m02d = m02Mat.Determinant();
	float32 m03d = m03Mat.Determinant();

	float32 m10d = m10Mat.Determinant();
	float32 m11d = m11Mat.Determinant();
	float32 m12d = m12Mat.Determinant();
	float32 m13d = m13Mat.Determinant();

	float32 m20d = m20Mat.Determinant();
	float32 m21d = m21Mat.Determinant();
	float32 m22d = m22Mat.Determinant();
	float32 m23d = m23Mat.Determinant();

	float32 m30d = m30Mat.Determinant();
	float32 m31d = m31Mat.Determinant();
	float32 m32d = m32Mat.Determinant();
	float32 m33d = m33Mat.Determinant();

	// 行列式
	float32 fDeterminant = 1.0F / (data[0] * m00d - data[1] * m01d +
		data[2] * m02d - data[3] * m03d);

	Matrix4x4 temp(
		m00d, -m10d, m20d, -m30d,
		-m01d, m11d, -m21d, m31d,
		m02d, -m12d, m22d, -m32d,
		-m03d, m13d, -m23d, m33d);

	return temp * fDeterminant;
}


Vector3 Matrix4x4::MultiplyVector3(const Vector3& vector)
{
	Vector3 temp;
	temp.x = data[0] * vector.x + data[1] * vector.y + data[2] * vector.z;
	temp.y = data[5] * vector.x + data[6] * vector.y + data[7] * vector.z;
	temp.z = data[9] * vector.x + data[10] * vector.y + data[11] * vector.z;
	return temp;
}

Vector4 Matrix4x4::MultiplyVector4(const Vector4& vector)
{
	Vector4 temp;
	temp.x = data[0] * vector.x + data[1] * vector.y + data[2] * vector.z + data[3] * vector.w;
	temp.y = data[4] * vector.x + data[5] * vector.y + data[6] * vector.z + data[7] * vector.w;
	temp.z = data[8] * vector.x + data[9] * vector.y + data[10] * vector.z + data[11] * vector.w;
	temp.w = data[12] * vector.x + data[13] * vector.y + data[14] * vector.z + data[15] * vector.w;

	return temp;
}

// 列
Vector4 Matrix4x4::GetColumn(int32 column)
{
	Vector4 temp;
	temp.x = Get(0, column);
	temp.y = Get(1, column);
	temp.z = Get(2, column);
	temp.w = Get(3, column);
	return temp;
}

// 获取行
Vector4 Matrix4x4::GetRow(int32 row)
{
	Vector4 temp;
	temp.x = Get(row, 0);
	temp.y = Get(row, 1);
	temp.z = Get(row, 2);
	temp.w = Get(row, 3);
	return temp;
}

void Matrix4x4::SetPosition(const Vector4& position)
{
	data[3] = position.x;
	data[7] = position.y;
	data[11] = position.z;
	data[15] = position.w;
}

void Matrix4x4::SetPosition(const Vector3& position)
{
	data[3] = position.x;
	data[7] = position.y;
	data[11] = position.z;
	data[15] = 1;
}


void Matrix4x4::SetScale(Vector3& scale)
{
	data[0] *= scale.x;
	data[1] *= scale.y;
	data[2] *= scale.z;

	data[4] *= scale.x;
	data[5] *= scale.y;
	data[6] *= scale.z;

	data[8] *= scale.x;
	data[9] *= scale.y;
	data[10] *= scale.z;
	data[15] = 1;
}

void Matrix4x4::SetScale(const float32& x, const float32& y, const float32& z)
{
	data[0] = x;
	data[5] = y;
	data[10] = z;
	data[15] = 1;
}

void Matrix4x4::SetScale(float32& x, float32& y, float32& z)
{
	data[0]  = x;
	data[5]  = y;
	data[10] = z;
	data[15] = 1;
}

void Matrix4x4::SetRotate(const Vector3& euler)
{
	EulerToMatrix(Vector4(euler.x, euler.y, euler.z), *this);
}

void Matrix4x4::SetRotate(const Quaternion& q)
{
	QuaternionToMatrix(q, *this);
}

void Matrix4x4::SetTRS(const Vector3& pos, const Quaternion& q, const Vector3& scale)
{
	QuaternionToMatrix(q, *this);
	data[0] *= scale.x;
	data[1] *= scale.x;
	data[2] *= scale.x;
	data[3] = pos.x;

	data[4] *= scale.y;
	data[5] *= scale.y;
	data[6] *= scale.y;
	data[7] = pos.y;

	data[8] *= scale.z;
	data[9] *= scale.z;
	data[10] *= scale.z;
	data[11] = pos.z;

	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 1;
}

void Matrix4x4::SetTRS(const Vector4& pos, const Quaternion& q, const Vector3& scale)
{
	QuaternionToMatrix(q, *this);
	
	data[0] *= scale.x;
	data[1] *= scale.x;
	data[2] *= scale.x;
	data[3] = pos.x;

	data[4] *= scale.y;
	data[5] *= scale.y;
	data[6] *= scale.y;
	data[7] = pos.y;

	data[8] *= scale.z;
	data[9] *= scale.z;
	data[10] *= scale.z;
	data[11] = pos.z;

	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 1;
}

void Matrix4x4::SetTR(const Vector4& pos, const Quaternion& q)
{
	QuaternionToMatrix(q, *this);

	data[3] = pos.x;
	data[7] = pos.y;
	data[11] = pos.z;

	data[12] = 0;
	data[13] = 0;
	data[14] = 0;
	data[15] = 1;
}

Vector3 Matrix4x4::GetPositionV3()const
{
	Vector3 temp;
	temp.x = data[3];
	temp.y = data[7];
	temp.z = data[11];

	return temp;
}

Vector4 Matrix4x4::GetPositionV4()const
{
	Vector4 temp;
	temp.x = data[3];
	temp.y = data[7];
	temp.z = data[11];
	temp.w = 1;

	return temp;
}

// https://blog.csdn.net/hunter_wwq/article/details/21473519
Vector3 Matrix4x4::GetScale()const
{
	if (data[1] == 0 && data[2] == 0 &&
		data[4] == 0 && data[6] == 0 &&
		data[8] == 0 && data[9] == 0)
	{
		return Vector3(data[0], data[5], data[10]);
	}

	float32 x = data[0] * data[0] + data[1] * data[1] + data[2] * data[2];
	float32 y = data[4] * data[4] + data[5] * data[5] + data[6] * data[6];
	float32 z = data[8] * data[8] + data[9] * data[9] + data[10] * data[10];

	// 我们必须进行完整的计算
	return Vector3(Sqrt(x), Sqrt(y), Sqrt(z));
}

Vector3 Matrix4x4::GetRotate()const
{
	Vector4 rotate;
	MatrixToEuler(*this, rotate);

	float32 x = float32(rotate.x * RAD_TO_DEG);
	float32 y = float32(rotate.y * RAD_TO_DEG);
	float32 z = float32(rotate.z * RAD_TO_DEG);

	return Vector3(x, y, z);
}

//----------------------角度表达式转换——-----------------//
void EulerToMatrix(const Vector4& inV, Matrix4x4& outMat)
{
	float32 x = float32(inV.x * DEG_TO_RAD);
	float32 y = float32(inV.y * DEG_TO_RAD);
	float32 z = float32(inV.z * DEG_TO_RAD);

	float32 cp = Cos(x);
	float32 sp = Sin(x);

	float32 ch = Cos(y);
	float32 sh = Sin(y);

	float32 cb = Cos(z);
	float32 sb = Sin(z);

	outMat[0] = ch * cb + sh * sp * sb;
	outMat[1] = -ch * sb + sh * sp * cb;
	outMat[2] = sh * cp;
	outMat[3] = 0;

	outMat[4] = sb * cp;
	outMat[5] = cb * cp;
	outMat[6] = -sp;
	outMat[7] = 0;

	outMat[8] = -sh * cb + ch * sp * sb;
	outMat[9] = sb * sh + ch * sp * cb;
	outMat[10] = ch * cp;
	outMat[11] = 0;

	outMat[12] = 0;
	outMat[13] = 0;
	outMat[14] = 0;
	outMat[15] = 1;
}
/// <summary>
/// 公式
/// 
/// |cosh*cosb+sinh*sinp*sinb     -cosh*sinb+sinh*sinp*cosb    sinh*cosp |
/// |       sinb*cosp                   cosb*cosp                -sinp   |
/// |-sinh*cosb+cosh*sinp*sinb     sinb*sinh+cosh*sinp*cosb    cosh*cosp |
/// 
/// </summary>
void MatrixToEuler(const Matrix4x4& inMat, Vector4& outV)
{
	if (inMat[6] > 0.999999F)  // 允许一些误差
	{
		outV.x = float32(PI * -0.5F);
		outV.y = ATan2(inMat[2], inMat[10]);
		outV.z = 0;
	}
	else if (inMat[6] < -0.999999F)
	{
		outV.x = float32(PI * 0.5F);
		outV.y = ATan2(inMat[2], inMat[10]);
		outV.z = 0;
	}
	else
	{
		outV.x = ASin(inMat[6]);
		outV.y = ATan2(inMat[2], inMat[10]);
		outV.z = ATan2(inMat[4], inMat[5]);
	}
}


void QuaternionToMatrix(const Quaternion& inQ, Matrix4x4& outMat)
{
	outMat.SetIdentity();

	float32 xx2 = inQ.x * inQ.x * 2;
	float32 yy2 = inQ.y * inQ.y * 2;
	float32 zz2 = inQ.z * inQ.z * 2;

	float32 xy2 = inQ.x * inQ.y * 2;
	float32 wz2 = inQ.w * inQ.z * 2;

	float32 xz2 = inQ.x * inQ.z * 2;
	float32 wy2 = inQ.w * inQ.y * 2;

	float32 yz2 = inQ.y * inQ.z * 2;
	float32 wx2 = inQ.w * inQ.x * 2;

	outMat[0] = 1 - yy2 - zz2;
	outMat[1] = xy2 + wz2;
	outMat[2] = xz2 - wy2;

	outMat[4] = xy2 - wz2;
	outMat[5] = 1 - xx2 - zz2;
	outMat[6] = yz2 + wx2;

	outMat[8] = xz2 + wy2;
	outMat[9] = yz2 - wx2;
	outMat[10] = 1 - xx2 - yy2;

	outMat[12] = 0;outMat[13] = 0;outMat[14] = 0;
	outMat[3] = 0; outMat[7] = 0; outMat[11] = 0; outMat[15] = 1;
}

//---------------------------------投影矩阵---------------------//

// 正交投影
Matrix4x4  MatrixOrthoLH(
	float32 left, float32 right,
	float32 top, float32 bottom,
	float32 zNear, float32 zFar)
{
	Matrix4x4 ortho;
	ortho[0] = 2 / (right - left);
	ortho[1] = 0;
	ortho[2] = 0;
	ortho[3] = -((right + left) / (right - left));

	ortho[4] = 0;
	ortho[5] = 2 / (top - bottom);
	ortho[6] = 0;
	ortho[7] = -((top + bottom) / (top - bottom));

	ortho[8] = 0;
	ortho[9] = 0;
	ortho[10] = 1 / (zFar - zNear);
	ortho[11] = -(zNear / (zFar - zNear));

	ortho[12] = 0;
	ortho[13] = 0;
	ortho[14] = 0;
	ortho[15] = 1;

	return ortho;
}

/// <summary>
/// 透视投影
/// </summary>
/// <param name="fov">传入角度而非弧度</param>
/// <param name="aspect"></param>
/// <param name="zNear"></param>
/// <param name="zFar"></param>
/// <returns></returns>
Matrix4x4  MatrixPerspectiveFovLH(
	float32 fov, 
	float32 aspect, 
	float32 zNear,
	float32 zFar)
{
	float32 rad = float32(fov * 0.5F * DEG_TO_RAD);
	float32 tan = Tan(rad, false);

	float32 h = 2 * zNear * tan;
	float32 w = 2 * zNear * aspect * tan;

	Debug::LogWarning("近裁剪平面H=" + Convert(h) + ",W=" + Convert(w));

	Matrix4x4 mat;
	mat[0] = 1.0F / (aspect * tan);
	mat[1] = 0;
	mat[2] = 0;
	mat[3] = 0;

	mat[4] = 0;
	mat[5] = 1 / tan;
	mat[6] = 0;
	mat[7] = 0;

	mat[8] = 0;
	mat[9] = 0;
	mat[10] = zFar / (zFar - zNear);
	mat[11] = (zFar * zNear) / (zNear - zFar);

	mat[12] = 0;
	mat[13] = 0;
	mat[14] = 1;
	mat[15] = 0;

	return mat;
}

void TransformBoxEx(Matrix4x4& mat, AABB& box)
{
	Vector4 minV(box.min.x, box.min.y, box.min.z, 1);
	Vector4 maxV(box.max.x, box.max.y, box.max.z, 1);

	box.min = mat.MultiplyVector4(minV);
	box.max = mat.MultiplyVector4(maxV);
}

// aabb变换
// 参考知乎 = https://zhuanlan.zhihu.com/p/116051685
void TransformBox(Matrix4x4& mat, AABB& box)
{
	Vector4 col0 = mat.GetColumn(0);
	Vector4 col1 = mat.GetColumn(1);
	Vector4 col2 = mat.GetColumn(2);
	Vector4 col3 = mat.GetColumn(3);

	Vector4 xa = col0 * box.min.x;
	Vector4 xb = col0 * box.max.x;

	Vector4 ya = col1 * box.min.y;
	Vector4 yb = col1 * box.max.x;

	Vector4 za = col2 * box.min.z;
	Vector4 zb = col2 * box.max.z;

	float32 w = mat.Get(3, 3);

	Vector4 pMin = Min(xa, xb) + Min(ya, yb) + Min(za, zb) + col3;
	Vector4 pMax = Max(xa, xb) + Max(ya, yb) + Max(za, zb) + col3;

	box.min.x = pMin.x / w;
	box.min.y = pMin.y / w;
	box.min.z = pMin.z / w;

	box.max.x = pMax.x / w;
	box.max.y = pMax.y / w;
	box.max.z = pMax.z / w;
}