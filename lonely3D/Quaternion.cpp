#include "Quaternion.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix3x3.h"
#include "Matrix4x4.h"


float32 Quaternion::Dot(Quaternion inVec)
{
	return x * inVec.x + y * inVec.y + z * inVec.z + w * inVec.w;
}

float32 Quaternion::Magnitude()
{
	return Sqrt(Dot(*this));
}

float32 Quaternion::SqrMagnitude()
{
	return Dot(*this);
}

void Quaternion::Normalize()
{
	float32 magnitude = Magnitude();
	x /= magnitude;
	y /= magnitude;
	z /= magnitude;
	w /= magnitude;
}

void Quaternion::Set(float32 newX, float32 newY, float32 newZ, float32 newW)
{
	x = newX; y = newY; z = newZ; w = newW;
}

Quaternion Quaternion::Inverse()
{
	return Conjugation() / Magnitude();
}

Quaternion Quaternion::Conjugation()
{
	return Quaternion(-x, -y, -z, w);
}

Quaternion Euler(Vector3 euler)
{
	Quaternion temp;
	EulerToQuaternion(euler, temp);
	return temp;
}


//------------------- 表达式转换---------------------//
void  EulerToQuaternion(const Vector3& eulerAngle, Quaternion& outQ)
{
	// y,x,z 各轴四元数相乘
	float32 halfAngle = float32(DEG_TO_RAD * 0.5F);

	float32 ch = Cos(eulerAngle.y * halfAngle);
	float32 sh = Sin(eulerAngle.y * halfAngle);

	float32 cp = Cos(eulerAngle.x * halfAngle);
	float32 sp = Sin(eulerAngle.x * halfAngle);

	float32 cb = Cos(eulerAngle.z * halfAngle);
	float32 sb = Sin(eulerAngle.z * halfAngle);

	//Quaternion h(0, -sh,0 ,ch);
	//Quaternion p(-sp, 0, 0, cp);
	//Quaternion b(0, 0, -sb, cb);
	//outQ = h * p * b;

	outQ.w = ch * cp * cb + sh * sp * sb;
	outQ.x = -ch * sp * cb - sh * cp * sb;
	outQ.y = ch * sp * sb - sh * cp * cb;
	outQ.z = sh * sp * cb - ch * cp * sb;
}


void QuaternionToEuler(const Quaternion& inQ, Vector3& outV)
{
	float32 x = inQ.x;
	float32 y = inQ.y;
	float32 z = inQ.z;
	float32 w = inQ.w;

	float32 sp = -2.0F * (y * z + w * x);

	if (sp > 0.999999F)
	{
		outV.x = float32(PI * -0.5F);
		outV.y = ATan2(2 * (x*z - w * y), 1 - 2 * Pow(x, 2) - 2 * Pow(y, 2));
		outV.z = 0.0F;
	}
	else if (sp < -0.99999F)
	{
		outV.x = float32(PI * 0.5F);
		outV.y = ATan2(2 * (x*z - w * y), 1 - 2 * Pow(x, 2) - 2 * Pow(y, 2));
		outV.z = 0.0F;
	}
	else
	{
		outV.x = ASin(sp);
		outV.y = ATan2(2 * (x*z - w * y), 1 - 2 * Pow(x, 2) - 2 * Pow(y, 2));
		outV.z = ATan2(2 * (x*y - w * z), 1 * 2 * Pow(x, 2) - 2 * Pow(z, 2));
	}

	outV.x = float32(outV.x * RAD_TO_DEG);
	outV.y = float32(outV.y * RAD_TO_DEG);
	outV.z = float32(outV.z * DEG_TO_RAD);

}

void MatrixToQuaternion(const Matrix4x4& inMat, Quaternion& outQ)
{
	Matrix3x3 temp;
	temp = inMat;
	MatrixToQuaternion(temp, outQ);
}

void MatrixToQuaternion(const Matrix3x3& inMat, Quaternion& outQ)
{
	float32 w4 =  inMat[0] + inMat[4] + inMat[8];
	float32 x4 =  inMat[0] - inMat[4] - inMat[8];
	float32 y4 = -inMat[0] + inMat[4] - inMat[8];
	float32 z4 = -inMat[0] - inMat[4] + inMat[8];

	float32 xy4 = inMat[1] + inMat[3];
	float32 wz4 = inMat[1] - inMat[3];

	float32 xz4 = inMat[6] + inMat[2];
	float32 wy4 = inMat[6] - inMat[2];

	float32 yz4 = inMat[5] + inMat[7];
	float32 wx4 = inMat[5] - inMat[7];

	int32 index = 0;
	float32 fRoot = w4;
	if (fRoot < x4)
	{
		index = 1;
		fRoot = x4;
	}
	if (fRoot < y4)
	{
		index = 2;
		fRoot = y4;
	}
	if (fRoot < z4)
	{
		index = 3;
		fRoot = z4;
	}

	switch (index)
	{
	case 0:
	{
		outQ.w = Sqrt(w4 + 1) * 0.5F;
		float32 fWWWW = 1.0F / (outQ.w * 4);
		outQ.x = wx4 * fWWWW;
		outQ.y = wy4 * fWWWW;
		outQ.z = wz4 * fWWWW;
	}break;
	case 1:
	{
		outQ.x = Sqrt(x4 + 1) * 0.5F;
		float32 fXXXX = 1.0F / (outQ.x * 4);
		outQ.w = wx4 * fXXXX;
		outQ.y = xy4 * fXXXX;
		outQ.z = xz4 * fXXXX;
	}break;
	case 2:
	{
		outQ.y = Sqrt(y4 + 1) * 0.5F;
		float32 fYYYY = 1.0F / (outQ.y * 4);
		outQ.x = xy4 * fYYYY;
		outQ.z = yz4 * fYYYY;
		outQ.w = wy4 * fYYYY;
	}break;
	case 3:
	{
		outQ.z = Sqrt(z4 + 1) * 0.5F;
		float32 fZZZZ = 1.0F / (outQ.z * 4);
		outQ.x = xz4 * fZZZZ;
		outQ.y = yz4 * fZZZZ;
		outQ.w = wz4 * fZZZZ;
	}break;
	}

}

void  QuaternionToMatrix(const Quaternion& inQ, Matrix3x3& outMat)
{
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

}



