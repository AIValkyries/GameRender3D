#pragma once
#ifndef __BoneAssignment_H__
#define __BoneAssignment_H__

#include "DataType.h"
#include "lMathf.h"

// 蒙皮绑定，权重
// 骨骼和顶点的绑定
struct BoneAssignment
{
	BoneAssignment() :BufferID(0),BoneID(0), VertexID(0), Weight(0.0F) {}
	uint32 BufferID;
	uint16  BoneID;
	int32  VertexID;
	float32 Weight;

	// 存储 VertexID 的顶点和法线
	Vector4 StaticPos;
	Vector3 StaticNormal;
	bool *Move;
};

#endif

