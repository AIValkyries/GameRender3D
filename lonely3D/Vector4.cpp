#include "Vector4.h"
#include "Vector2.h"
#include "Vector3.h"

Vector4& Vector4::operator+=(Vector3& inVec)
{
	x += inVec.x;
	y += inVec.y;
	z += inVec.z;

	return *this;
}

//Vector4& Vector4::operator=(Vector2 inVec)
//{
//	x = inVec.x;
//	y = inVec.y;
//	z = 0;
//	w = 1;
//	return *this;
//}
//Vector4& Vector4::operator=(Vector3 inVec)
//{
//	x = inVec.x;
//	y = inVec.y;
//	z = inVec.z;
//	w = 1;
//	return *this;
//}

Vector4& Vector4::operator=(const Vector2& inVec)
{
	x = inVec.x;
	y = inVec.y;
	z = 0;
	w = 1;
	return *this;
}
Vector4& Vector4::operator=(const Vector3& inVec)
{
	x = inVec.x;
	y = inVec.y;
	z = inVec.z;
	w = 1;
	return *this;
}

float32 Vector4::Dot(const Vector4& inVec)
{
	return x*inVec.x + y * inVec.y + z * inVec.z + w * inVec.w;
}

float32 Vector4::Dot(const Vector3& inVec)
{
	float32 xx = x * inVec.x;
	float32 yy = y * inVec.y;
	float32 zz = z * inVec.z;

	return xx * yy * zz;
}

float32 Vector4::Magnitude()
{
	return Sqrt(Dot(*this));
}

float32 Vector4::SqrMagnitude()
{
	return Dot(*this);
}

void Vector4::Normalize()
{
	float32 man = Magnitude();
	x /= man;
	y /= man;
	z /= man;
	w /= man;
}

void Vector4::Set(float32 newX, float32 newY, float32 newZ, float32 newW)
{
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

void Vector4::Abs()
{
	x = AbsF(x);
	y = AbsF(y);
	z = AbsF(z);
	w = AbsF(w);
}



 
 
 



 

