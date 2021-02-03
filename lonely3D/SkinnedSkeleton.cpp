#include "SkinnedSkeleton.h"
#include "AABB.h"
#include "Debug.h"
#include "Vertex.h"
#include "Debug.h"
 
static string testBoneName = "left_lower_wing";

SkinnedSkeleton::SkinnedSkeleton()
{
	boundingBox = 0;
	currentAnimation = 0;
	playingAnimation = false;
	animationFrames = 0;
	startFrame = 0;
	endFrame = 0;
	interpolationMode = EIM_LINEAR;
}

SkinnedSkeleton::~SkinnedSkeleton()
{
	if (meshBuffer.size())
	{
		for (uint32 i = 0;i < meshBuffer.size();i++)
		{
			meshBuffer[i]->Destroy();
			if (meshBuffer[i])
			{
				delete meshBuffer[i];
				meshBuffer[i] = 0;
			}
		}
		meshBuffer.clear();
	}

	if (animations.size())
	{
		map<string, Animation*>::iterator itr;
		for (itr = animations.begin();itr != animations.end();itr++)
		{
			if (itr->second)
			{
				delete itr->second;
				itr->second = 0;
			}
		}
		animations.clear();
	}

	if (allBone.size())
	{
		for (uint32 i = 0;i < allBone.size();i++)
		{
			if (allBone[i])
			{
				delete allBone[i];
				allBone[i] = 0;
			}
		}
		allBone.clear();
	}

	if (verticesMoved.size())
	{
		for (uint32 i = 0;i < verticesMoved.size();i++)
		{
			if (verticesMoved[i])
			{
				delete verticesMoved[i];
				verticesMoved[i] = 0;
			}
		}
		verticesMoved.clear();
	}

	if (boundingBox)
	{
		delete boundingBox;
		boundingBox = 0;
	}

	currentAnimation = 0;
}

float32 SkinnedSkeleton::PlayAnimation(string animName)
{
	if (animations.count(animName))
	{
		currentAnimation = animations[animName];
		playingAnimation = true;
		return currentAnimation->Length;
	}
	else
	{
		playingAnimation = false;
		return -1;   // 不存在名称
	}
	return 0;
}


/// <summary>
/// 根据动画关键帧更新骨骼
/// </summary>
/// <param name="frame"></param>
/// <param name="blend"></param>
void SkinnedSkeleton::UpdateBone(uint32 frame, float32 blend)
{
	Animation* animation = GetCurrentAnimation();
	if (animation == 0)
	{
		Debug::LogError("Animation 为空!!!!");
		return;
	}

	std::map<uint16, vector<PositionKeyFrame*>> positionKeys = animation->PostionKeyFrames;
	std::map<uint16, vector<ScaleKeyFrame*>>    scaleKeys = animation->ScaleKeyFrames;
	std::map<uint16, vector<RotateKeyFrame*>>   rotateKeys = animation->RotateKeyFrames;

	for (uint32 i = 0;i < allBone.size();i++)
	{
		vector<PositionKeyFrame*> posKey    = positionKeys[allBone[i]->BoneID];
		vector<ScaleKeyFrame*>    scaleKey  = scaleKeys[allBone[i]->BoneID];
		vector<RotateKeyFrame*>   rotateKey = rotateKeys[allBone[i]->BoneID];

		Vector4 oldPosition    = allBone[i]->Position;
		Vector3 oldScale       = allBone[i]->Scale;
		Quaternion oldRotation = allBone[i]->Rotate;

		Vector4 position = oldPosition;
		Vector3 scale = oldScale;
		Quaternion rotation = oldRotation;

		//Debug::LogInfo("------------------------------------------------");
		//Debug::LogInfo(allBone[i]->Name);
		//Debug::LogInfo(Convert(frame));
		//Debug::LogInfo("ROTATE=", rotation);

		TryGetFrameData(
			frame,
			posKey, scaleKey, rotateKey, 	
			position, scale, rotation,
			allBone[i]->PrePosIndex, 
			allBone[i]->PreScaleIndex, 
			allBone[i]->PreRotaionIndex);


	/*	if (allBone[i]->Name == testBoneName)*/
		//{
		//	Debug::LogInfo("------------------------------------------------");
		//	Debug::LogInfo("ROTATE=", rotation);
		//}

		if (blend != 1.0F)
		{
			allBone[i]->Position = Lerp(oldPosition, position, blend);
			allBone[i]->Scale  =   Lerp(oldScale, scale, blend);
			allBone[i]->Rotate =   Lerp(oldRotation, rotation, blend);
		}
		else
		{
			allBone[i]->Position = position;
			allBone[i]->Scale = scale;
			allBone[i]->Rotate = rotation;
		}
	}

	BuildAllLocalAnimatedMatrices();
}

/// <summary>
/// 对每个骨骼进行的操作
/// 获取到该帧的数据
/// </summary>
/// <param name="keyFrames">某一个骨骼的所有 关键帧</param>
/// <param name="frame">当前帧</param>
/// <param name="position">接收的数据</param>
/// <param name="scale"></param>
/// <param name="rotation"></param>
void SkinnedSkeleton::TryGetFrameData(
	uint32 frame,
	vector<PositionKeyFrame*> positionKeys,
	vector<ScaleKeyFrame*> scaleKeys,
	vector<RotateKeyFrame*> rotateKeys,
	Vector4& position,
	Vector3& scale,
	Quaternion& rotation,
	int32& prePosIndex,
	int32& preScaleIndex,
	int32& preRotaionIndex)
{
	int32 foundPosIndex = -1;
	int32 foundScaleIndex = -1;
	int32 foundRotationIndex = -1;

	if (positionKeys.size() != 0)
	{
		PositionKeyFrame* posKey = 0;

		// 如何上一帧不为空
		if (prePosIndex >= 0 && uint32(prePosIndex) < positionKeys.size())
		{
			// 在前一帧和当前帧的中间
			if (prePosIndex > 0 &&
				positionKeys[prePosIndex]->Frame > frame &&
				positionKeys[prePosIndex - 1]->Frame < frame)
			{
				foundPosIndex = prePosIndex;
			}
			else if (uint32((prePosIndex + 1)) < positionKeys.size())
			{
				// 在下一帧和当前帧的中间
				if (positionKeys[prePosIndex + 1]->Frame > frame &&
					positionKeys[prePosIndex]->Frame < frame)
				{
					prePosIndex++;
					foundPosIndex = prePosIndex;
				}
			}
		}

		if (foundPosIndex == -1)
		{
			for (uint32 i = 0;i < positionKeys.size();i++)
			{
				if (positionKeys[i]->Frame >= frame)
				{
					posKey = positionKeys[i];
					foundPosIndex = i;
					prePosIndex = i;
					break;
				}
			}
		}

		if (foundPosIndex != -1)
		{
			if (interpolationMode == EIM_CONSTANT || foundPosIndex == 0)
			{
				position = positionKeys[foundPosIndex]->Position;
			}
			else if (interpolationMode == EIM_LINEAR)  // 插值
			{
				PositionKeyFrame* maxKey = positionKeys[foundPosIndex];
				PositionKeyFrame* minKey = positionKeys[foundPosIndex - 1];

				float32 fd1 = frame - maxKey->Frame;
				float32 fd2 = minKey->Frame - frame;

				float32 inter = (float32)(fd1) / (fd1 + fd2);

				position = (maxKey->Position - minKey->Position) * inter + minKey->Position;
			}
		}

	}

	if (scaleKeys.size() != 0)
	{
		ScaleKeyFrame* scaleKey = 0;

		if (preScaleIndex >= 0 && uint32(preScaleIndex) < scaleKeys.size())
		{
			if (preScaleIndex > 0 && scaleKeys[preScaleIndex]->Frame > frame &&
				scaleKeys[prePosIndex - 1]->Frame < frame)
			{
				foundScaleIndex = preScaleIndex;
			}
			else if (uint32(preScaleIndex + 1) < scaleKeys.size())
			{
				if (scaleKeys[preScaleIndex + 1]->Frame > frame &&
					scaleKeys[preScaleIndex]->Frame < frame)
				{
					preScaleIndex++;
					foundScaleIndex = preScaleIndex;
				}
			}
		}

		if (foundScaleIndex == -1)
		{
			for (uint32 i = 0;i < scaleKeys.size();i++)
			{
				if (scaleKeys[i]->Frame >= frame)
				{
					foundScaleIndex = i;
					preScaleIndex = i;
					scaleKey = scaleKeys[i];
					break;
				}
			}
		}

		if (foundScaleIndex != -1)
		{
			if (interpolationMode == EIM_CONSTANT || foundScaleIndex == 0)
			{
				scale = scaleKeys[foundScaleIndex]->Scale;
			}
			else if (interpolationMode == EIM_LINEAR)
			{
				ScaleKeyFrame* maxKey = scaleKeys[foundScaleIndex];
				ScaleKeyFrame* minKey = scaleKeys[foundScaleIndex - 1];

				float32 fd1 = frame - maxKey->Frame;
				float32 fd2 = minKey->Frame - frame;

				float32 inter = (float32)(fd1) / (fd1 + fd2);

				scale = (maxKey->Scale - minKey->Scale) * inter + minKey->Scale;
			}
		}
	}

	if (rotateKeys.size() != 0)
	{
		RotateKeyFrame* rotateKey = 0;

		if (preRotaionIndex >= 0 && uint32(preRotaionIndex) < rotateKeys.size())
		{
			if (preRotaionIndex > 0 && rotateKeys[preRotaionIndex]->Frame > frame &&
				rotateKeys[preRotaionIndex - 1]->Frame < frame)
			{
				foundRotationIndex = preRotaionIndex;
			}
			else if (uint32(preRotaionIndex + 1) < rotateKeys.size())
			{
				if (rotateKeys[preRotaionIndex + 1]->Frame > frame &&
					rotateKeys[preRotaionIndex]->Frame < frame)
				{
					preRotaionIndex++;
					foundRotationIndex = preRotaionIndex;
				}
			}
		}

		//foundRotationIndex = -1;
		if (foundRotationIndex == -1)
		{
			for (uint32 i = 0;i < rotateKeys.size();i++)
			{
			/*	Quaternion Rotate = rotateKeys[i]->Rotate;*/
				if (rotateKeys[i]->Frame >= frame)
				{
					foundRotationIndex = i;
					preRotaionIndex = i;
					rotateKey = rotateKeys[i];
					break;
				}
			}
		}

		if (foundRotationIndex != -1)
		{
			if (interpolationMode == EIM_CONSTANT || foundRotationIndex == 0)
			{
				rotation = rotateKeys[foundRotationIndex]->Rotate;
			}
			else if (interpolationMode == EIM_LINEAR)
			{
				RotateKeyFrame* maxKey = rotateKeys[foundRotationIndex];
				RotateKeyFrame* minKey = rotateKeys[foundRotationIndex - 1];

				float32 fd1 = frame - maxKey->Frame;
				float32 fd2 = minKey->Frame - frame;

				float32 inter = (float32)(fd1) / (fd1 + fd2);

				rotation = Slerp(maxKey->Rotate, minKey->Rotate, inter);

				int32 count = 1;
			}
		}
	}
}

// 重新构建当前局部矩阵
void SkinnedSkeleton::BuildAllLocalAnimatedMatrices()
{
	Animation* animation = GetCurrentAnimation();

	std::map<uint16, vector<PositionKeyFrame*>> positionKeys = animation->PostionKeyFrames;
	std::map<uint16, vector<ScaleKeyFrame*>>    scaleKeys    = animation->ScaleKeyFrames;
	std::map<uint16, vector<RotateKeyFrame*>>   rotateKeys   = animation->RotateKeyFrames;

	for (uint32 i = 0;i < allBone.size();i++)
	{
		Bone* bone = allBone[i];

		vector<PositionKeyFrame*> pKeyFrames = positionKeys[bone->BoneID];
		vector<ScaleKeyFrame*> sKeyFrames = scaleKeys[bone->BoneID];
		vector<RotateKeyFrame*> rKeyFrames = rotateKeys[bone->BoneID];

		if (!pKeyFrames.size() &&
			!sKeyFrames.size() &&
			!rKeyFrames.size())
		{
			bone->LocalCurrentMatrix = bone->LocalBindMatrix;
		}
		else
		{
			// 将旋转变换成矩阵
			QuaternionToMatrix(bone->Rotate, bone->LocalCurrentMatrix);

			bone->LocalCurrentMatrix[3] = bone->Position.x;
			bone->LocalCurrentMatrix[7] = bone->Position.y;
			bone->LocalCurrentMatrix[11] = bone->Position.z;
			bone->LocalCurrentMatrix[15] = 1;

			if (sKeyFrames.size())
			{
				bone->LocalCurrentMatrix[0] *= bone->Scale.x;
				bone->LocalCurrentMatrix[1] *= bone->Scale.y;
				bone->LocalCurrentMatrix[2] *= bone->Scale.z;

				bone->LocalCurrentMatrix[4] *= bone->Scale.x;
				bone->LocalCurrentMatrix[5] *= bone->Scale.y;
				bone->LocalCurrentMatrix[6] *= bone->Scale.z;

				bone->LocalCurrentMatrix[8] *= bone->Scale.x;
				bone->LocalCurrentMatrix[9] *= bone->Scale.y;
				bone->LocalCurrentMatrix[10] *= bone->Scale.z;
			}
		}

	}

}

// 重新构建当前全局矩阵
void SkinnedSkeleton::BuildAllGlobalAnimatedMatrices()
{
	for (uint32 i = 0;i < allBone.size();i++)
	{
		Bone* bone = allBone[i];
		Bone* parent = 0;

		if (bone->Parent != 0xFFFF)
			parent = allBone[bone->Parent];

		if (!parent)
		{
			bone->GlobaCurrentMatrix = bone->LocalCurrentMatrix;
		}
		else
		{
			MultiplyMatrix(
				bone->GlobaCurrentMatrix,
				parent->GlobaCurrentMatrix,
				bone->LocalCurrentMatrix);
		}
	}

}

void SkinnedSkeleton::SkinMesh()
{
	BuildAllGlobalAnimatedMatrices();

	for (uint32 i = 0;i < meshBuffer.size();i++)
	{
		MeshBuffer* buffer = meshBuffer[i];

		int32 size = buffer->GetVertexCount();

		for (int32 j = 0;j < size;j++)
		{
			verticesMoved[i][j] = false;
		}
	}

	for (uint32 i = 0; i < allBone.size(); ++i)
	{
		Bone* bone = allBone[i];

		for (uint32 j = 0;j < bone->Weights.size();j++)
		{
			const uint16 buffer_id = uint16(bone->Weights[j]->BufferID);
			const uint32 vertex_id = uint16(bone->Weights[j]->VertexID);

			meshBuffer[buffer_id]->GetSourceVertex(vertex_id)->position =
				bone->Weights[j]->StaticPos;

			meshBuffer[buffer_id]->GetSourceVertex(vertex_id)->normal =
				bone->Weights[j]->StaticNormal;
		}
	}

	SkinBone();	
}

void SkinnedSkeleton::SkinBone()
{
	for (uint32 i = 0;i < allBone.size();i++)
	{
		Bone* bone = allBone[i];

		if (bone->Weights.size())
		{
			Matrix4x4 jointVertexPull;

			MultiplyMatrix(
				jointVertexPull,
				bone->GlobaCurrentMatrix,
				bone->GlobaBindInverseMatrix);

			static Vector4 vertexPos;
			static Vector4 vertexNormal;

			Vector4 thisVertexMove, thisNormalMove;

			for (uint32 j = 0;j < bone->Weights.size();j++)
			{
				BoneAssignment* weight = bone->Weights[j];
				MeshBuffer* LocalBuffers = meshBuffer[weight->BufferID];

				// get顶点
				Vertex* vertex = LocalBuffers->GetSourceVertex(weight->VertexID);

				thisVertexMove = jointVertexPull.MultiplyVector4(weight->StaticPos);
				thisNormalMove = jointVertexPull.MultiplyVector3(weight->StaticNormal);

				if (!(*(weight->Move)))
				{
					*(weight->Move) = true;
					vertex->position = thisVertexMove * weight->Weight;
					vertex->normal   = thisNormalMove * weight->Weight;
				}
				else
				{
					vertex->position += thisVertexMove * weight->Weight;
					vertex->normal.AddEqual(thisNormalMove * weight->Weight);
				}

			}
		}
	}
}


void SkinnedSkeleton::UpdateBoundingBox(Matrix4x4& mat)
{
	for (uint32 i = 0;i < meshBuffer.size();i++)
	{
		meshBuffer[i]->ResetAABB(0, 0, 0);
		AABB* aabb = meshBuffer[i]->RecalculateAABB();
		
		// 变换的作用是什么呢？
		// TransformBox(mat, *aabb);
	}
}

Animation* SkinnedSkeleton::GetAnimation(string name)
{
	return animations[name];
}

Animation* SkinnedSkeleton::GetCurrentAnimation()
{
	if (currentAnimation == 0)
	{
		Debug::LogError("没有设置当前动画!!!");
		return NULL;
	}

	return currentAnimation;
}


MeshBuffer* SkinnedSkeleton::AddMeshBuffer()
{
	MeshBuffer* mesh = new MeshBuffer();
	meshBuffer.push_back(mesh);
	return mesh;
}

vector<MeshBuffer*> SkinnedSkeleton::GetSkinMeshBuffer()
{
	return meshBuffer;
}

Animation* SkinnedSkeleton::AddAnimation(string name)
{
	Animation* a = new Animation();
	animations[name] = a;

	return a;
}

Bone* SkinnedSkeleton::AddBone()
{
	Bone* bone = new Bone();
	allBone.push_back(bone);
	return bone;
}

void SkinnedSkeleton::CalculationFrame(uint32 FramesPerSecond)
{
	map<string, Animation*>::iterator itr;

	for (itr = animations.begin();itr != animations.end();itr++)
	{
		Animation* animation = itr->second;

		std::map<uint16, vector<PositionKeyFrame*>> positionKeys = animation->PostionKeyFrames;
		std::map<uint16, vector<ScaleKeyFrame*>>   scaleKeys = animation->ScaleKeyFrames;
		std::map<uint16, vector<RotateKeyFrame*>>  rotateKeys = animation->RotateKeyFrames;

		if (positionKeys.size())
		{
			map<uint16, vector<PositionKeyFrame*>>::iterator pIter;

			for (pIter = positionKeys.begin();pIter != positionKeys.end();pIter++)
			{
				vector<PositionKeyFrame*> ps = pIter->second;

				for (uint32 i = 0;i < ps.size();i++)
				{
					if (ps[i]->Frame == -1)
						ps[i]->Frame = ps[i]->Time * FramesPerSecond;
				}
			}
		}

		if (scaleKeys.size())
		{
			map<uint16, vector<ScaleKeyFrame*>>::iterator sIter;

			for (sIter = scaleKeys.begin();sIter != scaleKeys.end();sIter++)
			{
				vector<ScaleKeyFrame*> ss = sIter->second;

				for (uint32 i = 0;i < ss.size();i++)
				{
					if (ss[i]->Frame == -1)
						ss[i]->Frame = ss[i]->Time * FramesPerSecond;
				}
			}
		}

		if (rotateKeys.size())
		{
			map<uint16, vector<RotateKeyFrame*>>::iterator rIter;
			for (rIter = rotateKeys.begin();rIter != rotateKeys.end();rIter++)
			{
				vector<RotateKeyFrame*> rs = rIter->second;

				for (uint32 i = 0;i < rs.size();i++)
				{
					if (rs[i]->Frame == -1)
						rs[i]->Frame = rs[i]->Time * FramesPerSecond;
				}
			}
		}	

	}

}

void SkinnedSkeleton::Finalize()
{
	for (uint32 i = 0;i < meshBuffer.size();i++)
	{
		MeshBuffer* buffer = meshBuffer[i];
		buffer->CreateFaceState();

		int32 size = buffer->GetVertexCount();
		bool* temp = new bool[size];
		verticesMoved.push_back(temp);
		
		for (int32 j = 0;j < size;j++)
		{
			temp[j] = false;
		}
	}

	for (uint32 i = 0;i < allBone.size();i++)
	{
		Bone* bone = allBone[i];

		for (uint32 j = 0;j < bone->Weights.size();j++)
		{
			const uint16 buffer_id = uint16(bone->Weights[j]->BufferID);
			const uint32 vertex_id = uint16(bone->Weights[j]->VertexID);

			bool* temp = verticesMoved[buffer_id];

			bone->Weights[j]->Move = &temp[vertex_id];
		}
	}

	// 初始化全部矩阵
	for (uint32 i = 0;i < allBone.size();i++)
	{
		Bone* current = allBone[i];
		int32 parentId = current->Parent;
		Bone* parent = 0;
		if (parentId != 0xFFFF)
			parent = allBone[current->Parent];

		if (parent != 0)
		{
			MultiplyMatrix(
				current->GlobalBindMatrix,
				parent->GlobalBindMatrix,
				current->LocalBindMatrix);
		}
		else
		{
			current->GlobalBindMatrix = current->LocalBindMatrix;
		}
		current->GlobaBindInverseMatrix = current->GlobalBindMatrix.Inverse();
		current->LocalCurrentMatrix = current->LocalBindMatrix;
		current->GlobaCurrentMatrix = current->GlobalBindMatrix;

		//Debug::LogInfo("------------------------------------");
		//Debug::LogInfo(current->Name);
		//Debug::LogInfo("局部绑定矩阵=",   current->LocalBindMatrix);
		//Debug::LogInfo("全局绑定逆矩阵=", current->GlobaBindInverseMatrix);
	}

	interpolationMode = EIM_LINEAR;

}