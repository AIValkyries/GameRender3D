#pragma once
#ifndef __Ray2D_H__
#define __Ray2D_H__

#include "lMathf.h"

class Ray2D
{
public:
	Ray2D() {}
	~Ray2D() {}

	Vector2 point;
	Vector2 normal;
};

#endif

