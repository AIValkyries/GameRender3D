#include "AABB.h"

bool AABB::IsInside(const Vector3& inPoint) const
{
	if (inPoint.x > min.x && inPoint.x < max.x)
	{
		if (inPoint.y > min.y && inPoint.y < max.y)
		{
			if (inPoint.z > min.z && inPoint.z < max.z)
				return true;
		}
	}

	return false;
}

void AABB::Set(const Vector3& pos)
{
	min = pos;
	max = pos;
}

void AABB::Set(const Vector4& pos)
{
	min = Vector3(pos.x, pos.y, pos.z);
	max = Vector3(pos.x, pos.y, pos.z);
}

void AABB::Set(const AABB* aabb)
{
	min = aabb->min;
	max = aabb->max;
}

void AABB::Set(
	float32 x0, float32 y0, float32 z0, 
	float32 x1, float32 y1, float32 z1)
{
	min = Vector3(x0, y0, z0);
	max = Vector3(x1, y1, z1);
}

void AABB::UnionPoint(const Vector4& vertex)
{
	if (vertex.x > max.x) max.x = vertex.x;
	if (vertex.y > max.y) max.y = vertex.y;
	if (vertex.z > max.z) max.z = vertex.z;

	if (vertex.x < min.x) min.x = vertex.x;
	if (vertex.y < min.y) min.y = vertex.y;
	if (vertex.z < min.z) min.z = vertex.z;
}

void AABB::UnionPoint(const Vector3& vertex)
{
	min.x = Min(vertex.x, min.x);
	min.y = Min(vertex.y, min.y);
	min.z = Min(vertex.z, min.z);

	max.x = Max(vertex.x, max.x);
	max.y = Max(vertex.y, max.y);
	max.z = Max(vertex.z, max.z);
}

void AABB::UnionPoint(const std::vector<Vector3*> vertexs)
{
	if (vertexs.size() == 0) return;

	for (uint32 i = 0;i < vertexs.size();i++)
	{
		UnionPoint(*vertexs[i]);
	}
}

void AABB::Merge(const AABB& aabb)
{
	min.x = Min(aabb.min.x, min.x);
	min.y = Min(aabb.min.y, min.y);
	min.z = Min(aabb.min.z, min.z);

	max.x = Max(aabb.max.x, max.x);
	max.y = Max(aabb.max.y, max.y);
	max.z = Max(aabb.max.z, max.z);
}

void AABB::GetVertices(Vector4* outVertices)
{
	outVertices[0].Set(max.x, max.y, max.z, 1);
	outVertices[1].Set(min.x, max.y, max.z, 1);
	outVertices[2].Set(min.x, max.y, min.z, 1);
	outVertices[3].Set(max.x, max.y, min.z, 1);
	outVertices[4].Set(max.x, min.y, max.z, 1);
	outVertices[5].Set(min.x, min.y, max.z, 1);
	outVertices[6].Set(min.x, min.y, min.z, 1);
	outVertices[7].Set(max.x, min.y, min.z, 1);
}

/*  1-------0
   /|      /|
  / |     / |
 2-------3  |
 |  5----|--4
 | /     | /
 |/      |/
 6-------7
 */
void AABB::GetVertices(Vector3* outVertices)
{
	outVertices[0].Set(max.x, max.y, max.z);
	outVertices[1].Set(min.x, max.y, max.z);
	outVertices[2].Set(min.x, max.y, min.z);
	outVertices[3].Set(max.x, max.y, min.z);
	outVertices[4].Set(max.x, min.y, max.z);
	outVertices[5].Set(min.x, min.y, max.z);
	outVertices[6].Set(min.x, min.y, min.z);
	outVertices[7].Set(max.x, min.y, min.z);
}

Vector3 AABB::GetSize()
{
	return max - min;
}

Vector3 AABB::GetCenter()
{
	return (max + min) * 0.5F;
}

Vector3 AABB::GetExtent()
{
	return (max - min) * 0.5F;
}