#pragma once
#ifndef __QuadTreeNode_H__
#define __QuadTreeNode_H__

#include "ISceneNode.h"
#include "SceneObject.h"
#include <vector>

class QuadTreeRect
{
public:
	int32 x0, x1;
	int32 y0, y1;
	int32 z0, z1;

	QuadTreeRect() 
	{
		x0 = 0;
		x1 = 0;
		y0 = 0;
		y1 = 0;
		z0 = 0;
		z1 = 0;
	}

	QuadTreeRect(
		float32 _x0, float32 _x1,
		float32 _y0, float32 _y1,
		float32 _z0, float32 _z1) 
	{
		x0 = int32(_x0);
		x1 = int32(_x1);

		y0 = int32(_y0);
		y1 = int32(_y1);

		z0 = int32(_z0);
		z1 = int32(_z1);
	}

	QuadTreeRect(
		int32 _x0, int32 _x1,
		int32 _y0, int32 _y1,
		int32 _z0, int32 _z1) :
		x0(_x0), x1(_x1),
		y0(_y0), y1(_y1),
		z0(_z0), z1(_z1)
	{

	}

	QuadTreeRect(const QuadTreeRect& src):
		x0(src.x0), x1(src.x1),
		y0(src.y0), y1(src.y1),
		z0(src.z0), z1(src.z1)
	{

	}

	~QuadTreeRect() {}

	void Convert(
		const AABB& box,
		const Vector3& offset,
		const Vector3& scale)
	{
		AABB worldBox(box);
		worldBox += offset;
		worldBox *= scale;

		x0 = int32(worldBox.min.x);
		y0 = int32(worldBox.min.y);
		z0 = int32(worldBox.min.z);

		x1 = int32(worldBox.max.x);
		y1 = int32(worldBox.max.y);
		z1 = int32(worldBox.max.z);

		x0 = Clamp(x0, 0, 255);
		y0 = Clamp(y0, 0, 255);
		z0 = Clamp(z0, 0, 255);

		x1 = Clamp(x1, x0 + 1, 255);
		y1 = Clamp(y1, y0 + 1, 255);
		z1 = Clamp(z1, z0 + 1, 255);
	}
};

class QuadTreeNode
{
public:

	void AddObject(SceneObject* sceneNode);
	void RemoveObject(SceneObject* sceneNode);
	bool HasObject(SceneObject* sceneNode);
	vector<ISceneNode*> SearchObject(
		const Frustum* pOptionalFrustum, int32& nodeCount);

	void Setup(
		int32 _index,
		int32 _level,
		int32 _x,
		int32 _y,
		QuadTreeNode* _parent,
		QuadTreeNode* child0,
		QuadTreeNode* child1,
		QuadTreeNode* child2,
		QuadTreeNode* child3);

	QuadTreeNode* GetChild(const int32 _index)const { return childs[_index]; }
	QuadTreeNode* GetParent()const { return parent; }
	int32 GetIndex()const { return  index; }
	int32 GetObjectCount()const { return allNodes.size(); }
	vector<SceneObject*> GetAllNodes()const { return allNodes; }

	const uint32 GetLevel()const { return level; }
	const int32 GetX()const { return x; }
	const int32 GetY()const { return y; }

	QuadTreeRect GetRect()const { return rect; }

	void TestNode(int32& totalCount);
	void SetQuadTreeRect(const QuadTreeRect _rect)
	{
		rect = _rect;
	}

private:
	QuadTreeNode* childs[4];
	QuadTreeNode* parent;
	vector<SceneObject*> allNodes;
	QuadTreeRect rect;
	int32 index;
	int32 level;
	int32 x;
	int32 y;

};

#endif

