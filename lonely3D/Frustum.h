#pragma once
#ifndef __Frustum_H__
#define __Frustum_H__

#include "Plane3D.h"
#include "AABB.h"
#include "Sphere.h"
#include "IntersectionTests.h"

class Frustum
{
public:
	Frustum();
	Frustum(const Frustum& src);
	~Frustum();

	Frustum& operator=(const Frustum& src);
	bool operator==(const Frustum& src);
	bool operator!=(const Frustum& src);

	Plane3D leftPlane;
	Plane3D rightPlane;
	Plane3D topPlane;
	Plane3D bottomPlane;
	Plane3D nearPlane;
	Plane3D farPlane;

	// 远裁剪平面 左上角
	Vector3 GetFarLeftUp()const;
	Vector3 GetFarRightUp()const;
	Vector3 GetFarLefthDown()const;
	Vector3 GetFarRightDown()const;

	Vector3 GetNearLeftUp()const;
	Vector3 GetNearRightUp()const;
	Vector3 GetNearLeftDown()const;
	Vector3 GetNearRightDown()const;

	const AABB* GetBoundingBox()const { return aabb; }
	void RecalculateBoundingBox();   //重新计算aabb
	void SetCameraPosition(Vector3& cp);
	void SetCameraPosition(Vector4& cp);
	 
	// 从矩阵中提取视锥体
	void ExtractFromMatrix(Matrix4x4& matrix, bool normalizePlanes = false);

	bool TestPoint(const Vector3& point)const;
	bool TestAABB(const AABB& aabb)const;
	bool TestSphere(const Sphere& sphere)const;

private:
	AABB* aabb;
	Vector3 cameraPosition;
};

inline Frustum::Frustum() 
{
	aabb = new AABB();
}
inline Frustum::Frustum(const Frustum& src)
{
	leftPlane = src.leftPlane;
	rightPlane = src.rightPlane;
	topPlane = src.topPlane;
	bottomPlane = src.bottomPlane;
	nearPlane = src.nearPlane;
	farPlane = src.farPlane;

	if (aabb) delete aabb;
	aabb = new AABB();
}
inline Frustum::~Frustum() {}

#pragma region operator

inline Frustum& Frustum::operator=(const Frustum& src)
{
	leftPlane = src.leftPlane;
	rightPlane = src.rightPlane;
	topPlane = src.topPlane;
	bottomPlane = src.bottomPlane;
	nearPlane = src.nearPlane;
	farPlane = src.farPlane;

	return *this;
}

inline bool Frustum::operator ==(const Frustum& src)
{
	return 
		leftPlane == src.leftPlane &&
		rightPlane == src.rightPlane &&
		topPlane == src.topPlane &&
		bottomPlane == src.bottomPlane &&
		nearPlane == src.nearPlane &&
		farPlane == src.farPlane;
}

inline bool Frustum::operator !=(const Frustum& src)
{
	return
		leftPlane != src.leftPlane ||
		rightPlane != src.rightPlane ||
		topPlane != src.topPlane ||
		bottomPlane != src.bottomPlane ||
		nearPlane != src.nearPlane ||
		farPlane != src.farPlane;
}

inline bool operator ==(const Frustum& f1,const Frustum& f2)
{
	return
		f1.leftPlane == f2.leftPlane &&
		f1.rightPlane == f2.rightPlane &&
		f1.topPlane == f2.topPlane &&
		f1.bottomPlane == f2.bottomPlane &&
		f1.nearPlane == f2.nearPlane &&
		f1.farPlane == f2.farPlane;
}


inline bool operator !=(const Frustum& f1, const Frustum& f2)
{
	return
		f1.leftPlane != f2.leftPlane ||
		f1.rightPlane != f2.rightPlane ||
		f1.topPlane != f2.topPlane ||
		f1.bottomPlane != f2.bottomPlane ||
		f1.nearPlane != f2.nearPlane ||
		f1.farPlane != f2.farPlane;
}

#pragma endregion

// 从矩阵中提取视锥体
// 平面方程 Ax+By+Cz+D=0;
inline void Frustum::ExtractFromMatrix(Matrix4x4& matrix, bool normalizePlanes)
{
	//Debug::LogInfo("视图投影矩阵=", matrix);

	Vector4 row1 = matrix.GetRow(0);
	Vector4 row2 = matrix.GetRow(1);
	Vector4 row3 = matrix.GetRow(2);
	Vector4 row4 = matrix.GetRow(3);

	leftPlane.normal.x = row4.x + row1.x;
	leftPlane.normal.y = row4.y + row1.y;
	leftPlane.normal.z = row4.z + row1.z;
	leftPlane.distance = (row4.w + row1.w);

	rightPlane.normal.x = row4.x - row1.x;
	rightPlane.normal.y = row4.y - row1.y;
	rightPlane.normal.z = row4.z - row1.z;
	rightPlane.distance = (row4.w - row1.w);

	bottomPlane.normal.x = row4.x + row2.x;
	bottomPlane.normal.y = row4.y + row2.y;
	bottomPlane.normal.z = row4.z + row2.z;
	bottomPlane.distance = (row4.w + row2.w);

	topPlane.normal.x = row4.x - row2.x;
	topPlane.normal.y = row4.y - row2.y;
	topPlane.normal.z = row4.z - row2.z;
	topPlane.distance = (row4.w - row2.w);

	nearPlane.normal.x = row3.x;
	nearPlane.normal.y = row3.y;
	nearPlane.normal.z = row3.z;
	nearPlane.distance = row3.w;

	farPlane.normal.x = row4.x - row3.x;
	farPlane.normal.y = row4.y - row3.y;
	farPlane.normal.z = row4.z - row3.z;
	farPlane.distance = (row4.w - row3.w);

	if (normalizePlanes)
	{
		float32 len = (1.0F / leftPlane.normal.Magnitude());
		leftPlane.normal *= len;
		leftPlane.distance *= len;

		len = (1.0F / rightPlane.normal.Magnitude());
		rightPlane.normal *= len;
		rightPlane.distance *= len;

		len = (1.0F / bottomPlane.normal.Magnitude());
		bottomPlane.normal *= len;
		bottomPlane.distance *= len;

		len = (1.0F / topPlane.normal.Magnitude());
		topPlane.normal *= len;
		topPlane.distance *= len;

		len = (1.0F / nearPlane.normal.Magnitude());
		nearPlane.normal *= len;
		nearPlane.distance *= len;

		len = (1.0F / farPlane.normal.Magnitude());
		farPlane.normal *= len;
		farPlane.distance *= len;
	}

	//Debug::LogInfo("------------------------------------------------------");
	//Debug::LogInfo("LEFT=" + leftPlane.ToString());
	//Debug::LogInfo("RIGHT=" + rightPlane.ToString());
	//Debug::LogInfo("TOP=" + topPlane.ToString());
	//Debug::LogInfo("BOTTOM=" + bottomPlane.ToString());
	//Debug::LogInfo("NEAR=" + nearPlane.ToString());
	//Debug::LogInfo("FAR=" + farPlane.ToString());

	// 重新计算 AABB
	RecalculateBoundingBox();
}

inline bool Frustum::TestPoint(const Vector3& point)const
{
	if (leftPlane.SignedDistance(point) < 0.0F ||
		rightPlane.SignedDistance(point) < 0.0F || 
		bottomPlane.SignedDistance(point) < 0.0F || 
		topPlane.SignedDistance(point) < 0.0F || 
		farPlane.SignedDistance(point) < 0.0F || 
		nearPlane.SignedDistance(point) < 0.0F )
	{
		return false;
	}
	return true;
}

inline bool Frustum::TestAABB(const AABB& aabb)const
{
	//int32 left_plane = Intersect(aabb, leftPlane);
	//int32 right_plane = Intersect(aabb, rightPlane);
	//int32 top_plane = Intersect(aabb, topPlane);
	//int32 bottom_plane = Intersect(aabb, bottomPlane);
	//int32 near_plane = Intersect(aabb, nearPlane);
	//int32 far_plane = Intersect(aabb, farPlane);

	//Debug::LogError("LEFT_PLANE=" + Convert(left_plane));
	//Debug::LogError("RIGHT_PLANE=" + Convert(right_plane));
	//Debug::LogError("TOP_PLANE=" + Convert(top_plane));
	//Debug::LogError("BOTTOM_PLANE=" + Convert(bottom_plane));
	//Debug::LogError("NEAR_PLANE=" + Convert(near_plane));
	//Debug::LogError("FAR_PLANE=" + Convert(far_plane));

	if (Intersect(aabb, leftPlane) == PLANE_BACK)
	{
		return false;
	}
	if (Intersect(aabb, rightPlane) == PLANE_BACK)
	{
		return false;
	}


	if (Intersect(aabb, topPlane) == PLANE_BACK)
	{
		return false;
	}
	if (Intersect(aabb, bottomPlane) == PLANE_BACK)
	{
		return false;
	}


	if (Intersect(aabb, nearPlane) == PLANE_BACK)
	{
		return false;
	}
	if (Intersect(aabb, farPlane)  == PLANE_BACK)
	{
		return false;
	}

	return true;
}

inline bool Frustum::TestSphere(const Sphere& sphere)const
{
	if (Intersect(sphere, leftPlane)   == PLANE_BACK ||
		Intersect(sphere, rightPlane)  == PLANE_BACK ||
		Intersect(sphere, topPlane)    == PLANE_BACK ||
		Intersect(sphere, bottomPlane) == PLANE_BACK ||
		Intersect(sphere, farPlane)    == PLANE_BACK ||
		Intersect(sphere, nearPlane)   == PLANE_BACK)
	{
		return false;
	}

	return true;
}

inline void Frustum::SetCameraPosition(Vector4& cp)
{
	cameraPosition.x = cp.x;
	cameraPosition.y = cp.y;
	cameraPosition.z = cp.z;
}

inline void Frustum::SetCameraPosition(Vector3& cp)
{
	cameraPosition = cp;
}

// 计算视锥体的 BOX
inline void Frustum::RecalculateBoundingBox()
{
	if (aabb == 0)return;
	aabb->Set(cameraPosition);

	aabb->UnionPoint(GetFarLeftUp());
	aabb->UnionPoint(GetFarRightUp());
	aabb->UnionPoint(GetNearLeftDown());
	aabb->UnionPoint(GetNearRightDown());

	//Debug::LogInfo("-------------------------------相机BOX-------------------------------");
	//Debug::LogInfo("MIN=", aabb->min);
	//Debug::LogInfo("MAX=", aabb->max);
}

inline Vector3 Frustum::GetFarLeftUp()const
{
	Vector3 p;

	Vector3 linePoint;
	Vector3 lineVect;

	if (IntersectPlane3D(farPlane, topPlane, linePoint, lineVect))
	{
		Intersect(leftPlane, linePoint, lineVect, p);
	}
 
	return p;
}

inline Vector3 Frustum::GetFarRightUp()const
{
	Vector3 p;

	Vector3 linePoint;
	Vector3 lineVect;

	if (IntersectPlane3D(farPlane, topPlane, linePoint, lineVect))
	{
		Intersect(rightPlane, linePoint, lineVect, p);
	}

	return p;
}

inline Vector3 Frustum::GetFarLefthDown()const
{
	Vector3 p;
	Vector3 linePoint;
	Vector3 lineVect;

	if (IntersectPlane3D(farPlane, bottomPlane, linePoint, lineVect))
	{
		Intersect(leftPlane, linePoint, lineVect, p);
	}

	//IntersectPlane3D(farPlane, leftPlane, bottomPlane, p);
	return p;
}

inline Vector3 Frustum::GetFarRightDown()const
{
	Vector3 p;
	Vector3 linePoint;
	Vector3 lineVect;

	if (IntersectPlane3D(farPlane, bottomPlane, linePoint, lineVect))
	{
		Intersect(rightPlane, linePoint, lineVect, p);
	}

	//IntersectPlane3D(farPlane, rightPlane, bottomPlane, p);
	return p;
}

 

inline Vector3 Frustum::GetNearLeftUp()const
{
	Vector3 p;
	Vector3 linePoint;
	Vector3 lineVect;

	if (IntersectPlane3D(nearPlane, topPlane, linePoint, lineVect))
	{
		Intersect(leftPlane, linePoint, lineVect, p);
	}
	//IntersectPlane3D(nearPlane, leftPlane, topPlane, p);
	return p;
}

inline Vector3 Frustum::GetNearRightUp()const
{
	Vector3 p;
	Vector3 linePoint;
	Vector3 lineVect;

	if (IntersectPlane3D(nearPlane, topPlane, linePoint, lineVect))
	{
		Intersect(rightPlane, linePoint, lineVect, p);
	}
	//IntersectPlane3D(nearPlane, rightPlane, topPlane, p);
	return p;
}

inline Vector3 Frustum::GetNearLeftDown()const
{
	Vector3 p;
	Vector3 linePoint;
	Vector3 lineVect;

	if (IntersectPlane3D(nearPlane, bottomPlane, linePoint, lineVect))
	{
		Intersect(leftPlane, linePoint, lineVect, p);
	}

	//IntersectPlane3D(nearPlane, leftPlane, bottomPlane, p);
	return p;
}

inline Vector3 Frustum::GetNearRightDown()const
{
	Vector3 p;
	Vector3 linePoint;
	Vector3 lineVect;

	if (IntersectPlane3D(nearPlane, bottomPlane, linePoint, lineVect))
	{
		Intersect(rightPlane, linePoint, lineVect, p);
	}
	//IntersectPlane3D(nearPlane, rightPlane, bottomPlane, p);
	return p;
}


#endif
