#pragma once
#ifndef __AABB_H__
#define __AABB_H__

#include "lMathf.h"
#include <vector>

class AABB
{
public:
	AABB();
	AABB(Vector3 _min, Vector3 _max);
	AABB(
		float32 x0, float32 x1, 
		float32 y0, float32 y1, 
		float32 z0, float32 z1)
	{
		min.x = x0;
		max.x = x1;

		min.y = y0;
		max.y = y1;

		min.z = z0;
		max.z = z1;
	}

	Vector3 min;
	Vector3 max;

	AABB& operator=(const AABB& aabb) { min = aabb.min;max = aabb.max; return *this; }
	bool operator==(const AABB& aabb) { return min == aabb.min && max == aabb.max; }
	bool operator!=(const AABB& aabb) { return min != aabb.min || max != aabb.max; }
	AABB& operator*=(const Vector3& inV);
	AABB& operator+=(const Vector3& inV);

	void Set(const Vector3& pos);
	void Set(const Vector4& pos);
	void Set(const AABB* aabb);
	void Set(float32 x0, float32 y0, float32 z0, float32 x1, float32 y1, float32 z1);
	bool IsInside(const Vector3& inPoint) const;
	void UnionPoint(const Vector3& vertex);
	void UnionPoint(const Vector4& vertex);
	void UnionPoint(const std::vector<Vector3*> vertexs);
	void Merge(const AABB& aabb);
	void GetVertices(Vector4* outVertices);
	void GetVertices(Vector3* outVertices);
	Vector3 GetSize();
	Vector3 GetCenter();
	Vector3 GetExtent();

	// º∆À„aabb 
	// aabb ±‰ªª  transform
};

inline AABB::AABB() {}
inline AABB::AABB(Vector3 _min, Vector3 _max) 
{
	min = _min;
	max = _max;
}


inline bool operator==(const AABB& aabb1, const AABB& aabb2)
{
	return aabb1.min == aabb2.min && aabb1.max == aabb2.max;
}

inline bool operator!=(const AABB& aabb1, const AABB& aabb2)
{
	return aabb1.min != aabb2.min || aabb1.max != aabb2.max;
}

inline AABB& AABB::operator*=(const Vector3& inV)
{
	min.x *= inV.x;
	max.x *= inV.x;
	min.y *= inV.y;
	max.y *= inV.y;
	min.z *= inV.z;
	max.z *= inV.z;

	return *this;
}

inline AABB& AABB::operator+=(const Vector3& inV)
{
	min.x += inV.x;
	max.x += inV.x;
	min.y += inV.y;
	max.y += inV.y;
	min.z += inV.z;
	max.z += inV.z;

	return *this;
}

 

#endif
