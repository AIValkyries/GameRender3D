#pragma once
#ifndef __Rect_H__
#define __Rect_H__

#include "DataType.h"
#include "lMathf.h"

class Rect
{
public:
	Rect();
	Rect(float32 _x, float32 _y, float32 _width, float32 _height)
		:x(_x), y(_y), width(_width), height(_height) {}
	~Rect();

	Rect& operator=(const Rect& rect);
	bool operator==(const Rect& rect);
	bool operator!=(const Rect& rect);

	float32 x, y, width, height;

	float32 GetXMax()const;
	float32 GetYMax()const;
	Vector2 GetMax()const;
	Vector2 GetMin()const;
	Vector2 GetCenter()const;
	Vector2 GetSize()const;

	void Set(float32 _x, float32 _y, float32 _width, float32 _height);
};

inline Rect::Rect()
{
	x = 0;
	y = 0;
	width = 0;
	height = 0;
}
inline Rect::~Rect() {}

#pragma region operator

inline Rect& Rect::operator=(const Rect& rect)
{
	x = rect.x;y = rect.y;
	width = rect.width;
	height = rect.height;

	return *this;
}

inline bool Rect::operator==(const Rect& rect)
{
	return x == rect.x && y == rect.y && rect.width == width && height == rect.height;
}

inline bool Rect::operator!=(const Rect& rect)
{
	return x != rect.x || y != rect.y || rect.width != width || height != rect.height;
}

inline bool operator ==(const Rect& rect1, const Rect& rect2)
{
	return rect1.x == rect2.x && 
		rect1.y == rect2.y && 
		rect1.width == rect2.width &&
		rect1.height == rect2.height;
}

inline bool operator !=(const Rect& rect1, const Rect& rect2)
{
	return rect1.x != rect2.x ||
		rect1.y != rect2.y ||
		rect1.width != rect2.width ||
		rect1.height != rect2.height;
}

#pragma endregion


inline float32 Rect::GetXMax()const
{
	return x + width;
}

inline float32 Rect::GetYMax()const
{
	return y + height;
}

inline Vector2 Rect::GetMax()const
{
	return Vector2(x + width, y + height);
}

inline Vector2 Rect::GetMin()const
{
	return Vector2(x, y);
}

inline Vector2 Rect::GetCenter()const
{
	Vector2 max = GetMax();
	Vector2 min = GetMin();

	return (max + min) * 0.5F;
}

inline Vector2 Rect::GetSize()const
{
	return Vector2(width, height);
}


inline void Rect::Set(float32 _x, float32 _y, float32 _width, float32 _height)
{
	x = _x;
	y = _y;
	width = _width;
	height = _height;
}

#endif
