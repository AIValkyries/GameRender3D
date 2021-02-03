#pragma once
#ifndef __AnimatedMeshSceneNode_H__
#define __AnimatedMeshSceneNode_H__

#include "SceneObject.h"
#include "Timer.h"
#include "SkinnedSkeleton.h"

// 动画帧
struct AnimatedFrame
{
	float32 StartFrame;
	float32 EndFrame;
	float32 FrameLength;
};

class AnimatedMeshSceneNode:public SceneObject
{
public:
	AnimatedMeshSceneNode(SkinnedSkeleton* mesh);
	~AnimatedMeshSceneNode();

	// 设置每秒多少帧
	void SetAnimateFramesPerSecond(uint32 frames);
	void SetSpeed(float32 animateSpeed);
	float32 GetSpeed()const;
	void SetLoop(bool loop)
	{
		looping = loop;
	}

	virtual ISceneNode* Create(GameHost* _gameHost);
	virtual void Destroy();
	virtual void Update(bool updateChild);
	virtual void Render(bool updateChild);

	void PlayAnimation(string name);
private:
	SkinnedSkeleton* mesh;

	AnimatedFrame animateFrame;

	float32 currentFrame;     // 当前帧
	uint32 framesPerSecond;   //每秒帧数

	float32 factor;         // 每帧时间
	float32 animateSpeed;   // 动画速度

	bool looping;
	bool playing;

	void BuildTime(Timer timer);
};

inline AnimatedMeshSceneNode::AnimatedMeshSceneNode(SkinnedSkeleton* _mesh)
{
	mesh = _mesh;
	looping = false;
	playing = false;
	animateFrame.StartFrame = 0;
	animateFrame.EndFrame = 0;
}

inline AnimatedMeshSceneNode::~AnimatedMeshSceneNode()
{

}



#endif

