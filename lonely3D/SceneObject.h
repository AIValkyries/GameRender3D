#pragma once
#ifndef __SceneObject_H__
#define __SceneObject_H__

#include "ISceneNode.h"
#include "Geometry.h"


class QuadTree;
class QuadTreeNode;
class SceneObject:public ISceneNode
{
public:
	virtual ISceneNode* Create(GameHost* _gameHost);
	virtual void Destroy();
	virtual void PreUpdate();
	virtual void Update(bool updateChild);
	virtual void UpdateBox(bool updateChild);
	virtual void Render(bool updateChild);
	virtual void RenderBox(bool updateChild);

	void AttachToQuadTree(QuadTree* tree);
	void DetachFromQuadTree();

	QuadTreeNode* GetQuadTreeNode() { return node; }
	const QuadTreeNode* GetQuadTreeNode()const { return node; }

protected:
	AABB* sourceBounds;
	QuadTreeNode* node;
	QuadTree* quadTree;

	// 如果物体的位置，缩放，旋转发生了改变，则重新计算在四叉树中的位置
	void RefreshQuadTreeMembership();
	void RecalcWorldBounds();

};

#endif

