#pragma once
#ifndef __SkinnedSkeleton_H__
#define __SkinnedSkeleton_H__

#include "DataType.h"
#include "Animation.h"
#include "Bone.h"
#include "BoneAssignment.h"
#include "KeyFrame.h"
#include "MeshBuffer.h"
#include "IResourceItem.h"
#include "Types.h"


class SkinnedSkeleton :public IResourceItem
{
public:
	SkinnedSkeleton();
	~SkinnedSkeleton();

	void UpdateBone(uint32 frame, float32 blend = 1.0F);
	// 蒙皮
	void SkinMesh();
	void Finalize();

	MeshBuffer* AddMeshBuffer();
	vector<MeshBuffer*> GetSkinMeshBuffer();
	Animation* AddAnimation(string name);
	Bone* AddBone();
	uint32 GetBoneSize()const { return allBone.size(); }
	vector<Bone*> GetAllBones() { return allBone; }
	map<string, Animation*> GetAllAnimation()const { return animations; }

	float32 PlayAnimation(string animName);
	Animation* GetAnimation(string name);
	void UpdateBoundingBox(Matrix4x4& mat);

	// 根据time 计算帧
	void CalculationFrame(uint32 FramesPerSecond);

private:
	Animation* GetCurrentAnimation();
	void TryGetFrameData(
		uint32 frame,
		vector<PositionKeyFrame*> positionKeys,
		vector<ScaleKeyFrame*> scaleKeys,
		vector<RotateKeyFrame*> rotateKeys,
		Vector4& position,
		Vector3& scale,
		Quaternion& rotation,
		int32& prePosIndex,
		int32& preScaleIndex,
		int32& preRotaionIndex);
	void BuildAllLocalAnimatedMatrices();
	void BuildAllGlobalAnimatedMatrices();

	// 蒙皮
	void SkinBone();

	/// <summary>
	/// 主要的三种数据
	/// 1.网格
	/// 2.骨骼
	/// 3.动画
	/// </summary>
	vector<MeshBuffer*> meshBuffer;
	map<string, Animation*> animations;
	vector<Bone*> allBone;
	vector<bool*> verticesMoved;
	AABB* boundingBox;
	Animation* currentAnimation;
	bool playingAnimation;

	uint32 animationFrames;
	uint32 startFrame;
	uint32 endFrame;

	INTERPOLATION_MODE interpolationMode;
};

#endif

