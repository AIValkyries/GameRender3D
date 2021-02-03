#pragma once
#ifndef __Animation_H__
#define __Animation_H__

#include "DataType.h"
#include "KeyFrame.h"
#include <vector>
#include <map>

struct Animation
{
	String Name;
	float32 Length;

	/// <summary>
	/// Key:BoneID
	/// Value:λ�ùؼ�֡
	/// </summary>
	map<uint16, vector<PositionKeyFrame*>> PostionKeyFrames;
	map<uint16, vector<ScaleKeyFrame*>>  ScaleKeyFrames;
	map<uint16,vector<RotateKeyFrame*>> RotateKeyFrames;
}; 

#endif

