#pragma once
#ifndef __Bone_H__
#define __Bone_H__

#include "DataType.h"
#include "lMathf.h"
#include "BoneAssignment.h"

struct Bone
{
	String Name;
	uint16 BoneID;
	uint16 Parent;

	vector<Bone*> Children;
	vector<BoneAssignment*> Weights;

	Vector4 Position;
	Quaternion Rotate;
	Vector3 Scale;

	// ∞Û∂®æÿ’Û
	Matrix4x4 LocalBindMatrix;
	Matrix4x4 GlobalBindMatrix;
	Matrix4x4 LocalCurrentMatrix;
	Matrix4x4 GlobaCurrentMatrix;
	Matrix4x4 GlobaBindInverseMatrix;

	int32 PrePosIndex;
	int32 PreScaleIndex;
	int32 PreRotaionIndex;
};

#endif

