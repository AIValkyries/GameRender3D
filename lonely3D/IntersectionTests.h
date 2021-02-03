#pragma once

#include "lMathf.h"
#include "Geometry.h"

enum PlaneClassifications
{
	PLANE_FRONT = 0,
	PLANE_BACK = 1,
	PLANE_INTERSECT = 2,
	PLANE_TANGENT = 3,
};

// 相交测试
inline bool Intersect(const AABB& a, const AABB& b)
{
	if (a.max < b.min)
		return false;
	return true;
}


/// 从包围盒中找到与平面法向量方向最近的对角线
/// 把对角线上的vMin,vMax代入平面参数方程，如果符号想法则相交
/// 参考 https://zhuanlan.zhihu.com/p/156750299
inline int32 Intersect(const AABB& aabb, const Plane3D& p1)
{
	Vector3 minPoint1(aabb.max);
	Vector3 maxPoint2(aabb.min);

	if (p1.normal.x > 0)
	{
		minPoint1.x = aabb.min.x;
		maxPoint2.x = aabb.max.x;
	}

	if (p1.normal.y > 0)
	{
		minPoint1.y = aabb.min.y;
		maxPoint2.y = aabb.max.y;
	}

	if (p1.normal.z > 0)
	{
		minPoint1.z = aabb.min.z;
		maxPoint2.z = aabb.max.z;
	}

	float32 min = p1.SignedDistance(minPoint1);

	if (min == 0)
	{
		return PLANE_TANGENT;
	}
	else if (min > 0)    // 如果min>0 则表示 在前面
	{
		return PLANE_FRONT;
	}

	float32 max = p1.SignedDistance(maxPoint2);
	if (max == 0)
	{
		return PLANE_TANGENT;
	}
	else if (max < 0)
	{
		return PLANE_BACK;          // 如果max<0 则表示，在后面
	}

	// 如果 BOX在 plane 的后面则表示 不相交
	return PLANE_INTERSECT;
}


inline bool Intersect(const Sphere& sa, const Sphere& sb)
{
	float32 radiusSum = sa.radius + sb.radius;

	Vector3 d = sa.center - sb.center;
	float32 dotDist = Dot(d, d);

	return dotDist <= (radiusSum * radiusSum);
}

inline bool Intersect(const Sphere& sa, const Plane3D& p1)
{
	float32 dist = p1.SignedDistance(sa.center);
	return AbsF(dist) < sa.radius;
}

// 平面与线段(直线相交)检测
// 平面方程 (Dot(n,X)=d)
// 直线方程 L=P+td
// 将直线方程代入 X, n*(P+td)=d
// 求解t  n*p+tn*d = d
// tn*d = d-n*p
// t = (d-n*p)/(n*d)
// d= 方向向量=lineVect P =起点向量 linePoint
inline bool Intersect(
	const Plane3D& p,
	const Vector3& linePoint,
	const Vector3& lineVect,
    Vector3& outIntersection)
{
	float32 t2 = Dot(p.normal, lineVect);
	if (t2 == 0) return false;

	float32 dot = Dot(p.normal, linePoint);

	float32 t = -(p.distance + dot) / t2;

	outIntersection = lineVect * t + linePoint;

	return true;
}

// 2平面相交检测,返回直线 lineVect = 方向向量 linePoint = 起点
// 平面方程 n*X = d, 直线方程 L=P+td
// 求两平面相交的linePoint, 该点跨越n1,n2,
// 替换X  n1*(k1*n1+k2*n2) = d1
//        n2*(k1*n1+k2*n2) = d2
//       线性方程组
//     k1(n1*n1)+k2(n1*n2) = d1
//     k1(n1*n2)+k2(n2*n2) = d2
// 利用 克莱姆法则求解 k1,k2
// k1 = d1(n2*n2)-d2(n1*n2)/(denom)
// k2 = d2(n1*n1)-d1(n1*n2)/(denom)
// denom = (n1*n1)(n2*n2)-(n1*n2)(n1*n2)
inline bool IntersectPlane3D(
	const Plane3D& p0, 
	const Plane3D& p1, 
	Vector3& linePoint,
	Vector3& lineVect)
{
	lineVect = Cross(p0.normal, p1.normal);

	// 两个平面是否平行
	if (Dot(lineVect, lineVect) < 0.0000000001F)return false; 

	float32 d11 = Sqrt(Dot(p0.normal, p0.normal));
	float32 d12 = Dot(p0.normal, p1.normal);
	float32 d22 = Sqrt(Dot(p1.normal, p1.normal));

	float32 denom = 1.0F / (d11 * d22 - d12 * d12);

	float32 k1 = (-p0.distance * d22 + p1.distance * d12) * denom;
	float32 k2 = (-p1.distance * d11 + p0.distance * d12) * denom;

	linePoint = p0.normal * k1 + p1.normal * k2;

	return true;
}

// 三平面相交测试 denom
inline bool IntersectPlane3D(
	const Plane3D& p0, 
	const Plane3D& p1, 
	const Plane3D& p2,
	Vector3& outPoint)
{
	Vector3 dir(p0.distance, p1.distance, p2.distance);

	Matrix3x3 D;
	D[0] = p0.normal.x;
	D[1] = p0.normal.y;
	D[2] = p0.normal.z;

	D[3] = p1.normal.x;
	D[4] = p1.normal.y;
	D[5] = p1.normal.z;

	D[6] = p2.normal.x;
	D[7] = p2.normal.y;
	D[8] = p2.normal.z;

	Matrix3x3 D1;
	D1[0] = dir.x;
	D1[1] = p0.normal.y;
	D1[2] = p0.normal.z;

	D1[3] = dir.y;
	D1[4] = p1.normal.y;
	D1[5] = p1.normal.z;

	D1[6] = dir.z;
	D1[7] = p2.normal.y;
	D1[8] = p2.normal.z;


	Matrix3x3 D2;
	D2[0] = p0.normal.x;
	D2[1] = dir.x;
	D2[2] = p0.normal.z;

	D2[3] = p1.normal.x;
	D2[4] = dir.y;
	D2[5] = p1.normal.z;

	D2[6] = p2.normal.x;
	D2[7] = dir.z;
	D2[8] = p2.normal.z;


	Matrix3x3 D3;
	D3[0] = p0.normal.x;
	D3[1] = p0.normal.y;
	D3[2] = dir.x;

	D3[3] = p1.normal.x;
	D3[4] = p1.normal.y;
	D3[5] = dir.y;

	D3[6] = p2.normal.x;
	D3[7] = p2.normal.y;
	D3[8] = dir.z;

	float32 d  = D.Determinant();
	float32 d1 = D1.Determinant();
	float32 d2 = D2.Determinant();
	float32 d3 = D3.Determinant();

	outPoint.x = d1 / d;
	outPoint.y = d2 / d;
	outPoint.z = d3 / d;

	return true;
}