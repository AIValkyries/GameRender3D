#include "Vector3.h"
#include "Vector2.h"
#include "Vector4.h"

Vector3::Vector3(const Vector2& inVec)
{
	x = inVec.x;y = inVec.y;z = 0;
}

Vector3::Vector3(const Vector3& inVec)
{
	x = inVec.x;y = inVec.y;z = inVec.z;
}

Vector3::Vector3(const Vector4& inVec)
{
	x = inVec.x;y = inVec.y;z = inVec.z;
}


Vector3& Vector3::operator()(Vector2 inVec)
{
	x = inVec.x; y = inVec.y;
	return *this;
}

Vector3& Vector3::operator()(Vector4 inVec)
{
	x = inVec.x; y = inVec.y; z = inVec.z;
	return *this;
}

Vector3& Vector3::operator =(Vector4 inVec)
{
	x = inVec.x; y = inVec.y; z = inVec.z;
	return *this;
}

Vector3& Vector3::operator =(Vector2 inVec)
{
	x = inVec.x; y = inVec.y;
	return *this;
}

//Vector3& Vector3::operator =(const Vector4& inVec)
//{
//	x = inVec.x; y = inVec.y; z = inVec.z;
//	return *this;
//}
//
//Vector3& Vector3::operator =(const Vector2& inVec)
//{
//	x = inVec.x; y = inVec.y;
//	return *this;
//}

float32 Vector3::Dot(const Vector3& inVec)
{
	return x * inVec.x + y * inVec.y + z * inVec.z;
}

float32 Vector3::Dot(const Vector4& inVec)
{
	float32 xx = x * inVec.x;
	float32 yy = y * inVec.y;
	float32 zz = z * inVec.z;

	return xx + yy + zz;
}

float32 Vector3::Magnitude()
{
	return Sqrt(Dot(*this));
}

float32 Vector3::SqrMagnitude()
{
	return Dot(*this);
}

void Vector3::Normalize()
{
	float32 length = x * x + y * y + z * z;
	if (length == 0)
		return;

	length = 1.0F / Sqrt(length);
	x = x * length;
	y = y * length;
	z = z * length;
}

void Vector3::Set(float32 newX, float32 newY, float32 newZ)
{
	x = newX; y = newY; z = newZ;
}

void Vector3::Set(const Vector4& inVec)
{
	x = inVec.x;
	y = inVec.y;
	z = inVec.z;
}

void Vector3::AddEqual(const Vector4& inVec)
{
	x += inVec.x;
	y += inVec.y;
	z += inVec.z;
}

void Vector3::AddEqual(const Vector3& inVec)
{
	x += inVec.x;
	y += inVec.y;
	z += inVec.z;
}
 

 
 


 