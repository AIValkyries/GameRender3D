#pragma once
#ifndef __KeyFrame_H__
#define __KeyFrame_H__

#include "DataType.h"
#include "lMathf.h"

// ¹Ø¼üÖ¡
struct PositionKeyFrame
{
	int32 BoneID;
	float32 Frame;
	float32 Time;
	Vector4 Position;

	PositionKeyFrame()
	{
		Frame = -1;
		BoneID = -1;
		Time = 0;
		Position = Vector4::zero();
	}
};

struct ScaleKeyFrame
{
	int32 BoneID;
	float32 Frame;
	float32 Time;
	Vector3 Scale;

	ScaleKeyFrame()
	{
		Frame = -1;
		BoneID = -1;
		Time = 0;
		Scale = Vector3::zero();
	}
};

struct RotateKeyFrame
{
	int32 BoneID;
	float32 Frame;
	float32 Time;
	Quaternion Rotate;

	RotateKeyFrame()
	{
		Frame = -1;
		BoneID = -1;
		Time = 0;
		Rotate = Quaternion::identity();
	}
};



#endif

