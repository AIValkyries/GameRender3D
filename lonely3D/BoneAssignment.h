#pragma once
#ifndef __BoneAssignment_H__
#define __BoneAssignment_H__

#include "DataType.h"
#include "lMathf.h"

// ��Ƥ�󶨣�Ȩ��
// �����Ͷ���İ�
struct BoneAssignment
{
	BoneAssignment() :BufferID(0),BoneID(0), VertexID(0), Weight(0.0F) {}
	uint32 BufferID;
	uint16  BoneID;
	int32  VertexID;
	float32 Weight;

	// �洢 VertexID �Ķ���ͷ���
	Vector4 StaticPos;
	Vector3 StaticNormal;
	bool *Move;
};

#endif

