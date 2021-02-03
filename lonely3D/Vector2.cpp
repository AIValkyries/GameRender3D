#include "Vector2.h"


float32 Vector2::Dot(Vector2 inVec)
{
	return  x * inVec.x + y * inVec.y;
}

float32 Vector2::Magnitude()
{
	return Sqrt(Dot(Vector2(x, y)));
}

float32 Vector2::SqrMagnitude()
{
	return Dot(Vector2(x, y));
}

void Vector2::Normalize()
{
	float32 magnitude = Magnitude();
	if (magnitude != 0)
	{
		x /= magnitude;
		y /= magnitude;
	}
}

void Vector2::Set(float32 newX, float32 newY)
{
	x = newX; y = newY;
}

float32 Vector2::Angle(Vector2 inVec)
{
	Normalize();
	inVec.Normalize();
	return float32(ACos(Dot(inVec)) * RAD_TO_DEG);
}

float32 Vector2::Distance(Vector2 inVec)
{
	Vector2 newV(x, y);
	newV -= inVec;
	return newV.Magnitude();
}






