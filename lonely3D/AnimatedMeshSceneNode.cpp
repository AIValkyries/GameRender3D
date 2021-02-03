#include "AnimatedMeshSceneNode.h"
#include "GameHost.h"

ISceneNode* AnimatedMeshSceneNode::Create(GameHost* _gameHost)
{
	SceneObject::Create(_gameHost);
	textureMulColor = true;

	worldBounds = new AABB();

	vector<MeshBuffer*> meshs = mesh->GetSkinMeshBuffer();

	for (uint32 i = 0;i < meshs.size();i++)
	{
		Texture* texture = meshs[i]->GetTexture();
		if (texture) texture->SetFilterMode(1);

		meshs[i]->ResetAABB(0, 0, 0);
		meshs[i]->RecalculateAABB();

		int32 vertexCount = meshs[i]->GetVertexCount();
		for (int32 j = 0;j < vertexCount;j++)
		{
			Vertex* sources = meshs[i]->GetSourceVertex(j);
			sources->color.Set(0.5F, 0.5F, 0.5F);
			sources->Index = j;
			worldBounds->UnionPoint(sources->position);
		}

		meshBuffers.push_back(meshs[i]);
	}

	return this;
}

// 设置每秒多少帧
void AnimatedMeshSceneNode::SetAnimateFramesPerSecond(uint32 frames)
{
	framesPerSecond = frames;
	// 计算帧
	mesh->CalculationFrame(framesPerSecond);
}

void AnimatedMeshSceneNode::SetSpeed(float32 _animateSpeed)
{
	animateSpeed = _animateSpeed;
	factor = (framesPerSecond / 1000.0F) * animateSpeed;
}

float32 AnimatedMeshSceneNode::GetSpeed()const
{
	return animateSpeed;
}

void AnimatedMeshSceneNode::PlayAnimation(string name)
{
	float32 length = mesh->PlayAnimation(name);
	if (length == -1)
		return;

	if (length <= 0.0F)
	{
		float32 endFrame = 0;
		float32 startFrame = 0;

		Animation* animation = mesh->GetAnimation(name);

		if (animation->PostionKeyFrames.size())
		{
			map<uint16, vector<PositionKeyFrame*>>::iterator iter;

			for (iter = animation->PostionKeyFrames.begin();
				 iter != animation->PostionKeyFrames.end();iter++)
			{
				vector<PositionKeyFrame*> pKeys = iter->second;

				for (uint32 i = 0;i < pKeys.size();i++)
				{
					if (startFrame < pKeys[i]->Frame)
					{
						startFrame = pKeys[i]->Frame;
					}
					if (endFrame > pKeys[i]->Frame)
					{
						endFrame = pKeys[i]->Frame;
					}
				}
			}
		}

		if (animation->RotateKeyFrames.size())
		{
			map<uint16, vector<RotateKeyFrame*>>::iterator iter;

			for (iter = animation->RotateKeyFrames.begin();
				iter != animation->RotateKeyFrames.end();iter++)
			{
				vector<RotateKeyFrame*> rKeys = iter->second;

				for (uint32 i = 0;i < rKeys.size();i++)
				{
					if (startFrame < rKeys[i]->Frame)
					{
						startFrame = rKeys[i]->Frame;
					}
					if (endFrame > rKeys[i]->Frame)
					{
						endFrame = rKeys[i]->Frame;
					}
				}

			}
		}

		if (animation->ScaleKeyFrames.size())
		{
			map<uint16, vector<ScaleKeyFrame*>>::iterator iter;

			for (iter = animation->ScaleKeyFrames.begin();
				iter != animation->ScaleKeyFrames.end();iter++)
			{
				vector<ScaleKeyFrame*> sKeys = iter->second;

				for (uint32 i = 0;i < sKeys.size();i++)
				{
					if (startFrame < sKeys[i]->Frame)
					{
						startFrame = sKeys[i]->Frame;
					}
					if (endFrame > sKeys[i]->Frame)
					{
						endFrame = sKeys[i]->Frame;
					}
				}
			}
		}

		animateFrame.EndFrame = startFrame;
		animateFrame.FrameLength = startFrame - endFrame;
	}
	else
	{
		animateFrame.EndFrame = length * framesPerSecond;
		animateFrame.FrameLength = animateFrame.EndFrame - animateFrame.StartFrame;
	}

	playing = true;
}

void AnimatedMeshSceneNode::BuildTime(Timer timer)
{
	float32 deltaTime = timer.GetDeltaTime();

	if (looping)
	{
		currentFrame += deltaTime * factor;

		if (animateSpeed > 0.0F)
		{
			if (currentFrame > animateFrame.EndFrame)
			{
				float32 length = currentFrame - animateFrame.EndFrame;
				float32 frameMod = fmod(length, animateFrame.EndFrame);
				currentFrame = animateFrame.StartFrame + frameMod;
			}
		}
		else
		{
			if (currentFrame < animateFrame.StartFrame)
			{
				float32 time = animateFrame.EndFrame - currentFrame;
				float32 fod = fmod(time, animateFrame.EndFrame);
				currentFrame = animateFrame.EndFrame - fod;
			}
		}
	}
	else
	{
		currentFrame += deltaTime * factor;

		if (animateSpeed > 0.0F)  // 正常播放
		{
			if (currentFrame > animateFrame.EndFrame)
			{
				playing = false;
				currentFrame = animateFrame.EndFrame;
			}
		}
		else
		{
			if (currentFrame < 0)
			{
				playing = false;
				currentFrame = 0;
			}
		}
	}
}

void AnimatedMeshSceneNode::Render(bool updateChild)
{
	if (GetNodeID() == 4)
	{
		int32 count = 1;
	}

	SceneObject::Render(updateChild);
}

void AnimatedMeshSceneNode::Update(bool updateChild)
{
	SceneObject::Update(updateChild);

	if (!playing)
		return;

	if (mesh)
	{
		//CurrentFrame
		//Mesh->UpdateBone(0, 1.0F);
		mesh->UpdateBone((uint32)currentFrame, 1.0F);
		mesh->SkinMesh();
		mesh->UpdateBoundingBox(localToWorldMatrix);

		vector<MeshBuffer*> buffers = mesh->GetSkinMeshBuffer();
		if (buffers.size())
		{		
			for (uint32 i = 0;i < buffers.size();i++)
			{
				int32 vertexCount = buffers[i]->GetVertexCount();

				for (int32 j = 0;j < vertexCount;j++)
				{
					Vertex* sources = buffers[i]->GetSourceVertex(j);
					worldBounds->UnionPoint(sources->position);
				}
			}
		}
	}

	BuildTime(*gameHost->GetTimer());
}


void AnimatedMeshSceneNode::Destroy()
{
	Debug::LogInfo("移除动画网格!!");
	SceneObject::Destroy();

	mesh = 0;
}

