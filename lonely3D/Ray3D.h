#pragma once
#ifndef __Ray3D_H__
#define __Ray3D_H__

#include "lMathf.h"

class Ray3D
{
public:
	Ray3D() {}
	~Ray3D() {}

	Vector3 point;
	Vector3 normal;
};

#endif
