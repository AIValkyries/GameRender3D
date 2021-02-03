#pragma once
#ifndef __QuadTree_H__
#define __QuadTree_H__

#include "QuadTreeNode.h"
#include <vector>

/// <summary>
/// 层级  分割平面间隔  间隔的二进制值  最高比特位
//   0       128          10000000          7
//   1       64           01000000          6
//   2       32           00100000          5
//   3       16           00010000          4
//   4       8            00001000          3
//   5       4            00000100          2
//   6       2            00000010          1
//   7       1            00000001          0
// 将四叉树规定为  256 X 256 各轴为 2^8  ，四叉树成偶数分割
// 1.将物体BOX转换成四叉树空间 最大为256 X 256
// 2. r0 = x0^x1 ,r1 = y0^y1, r = Max(r0,r1)  异或特性
// 3. maxLevel - r = currentLevel
/// </summary>
class QuadTree
{
public:

	enum Constants
	{
		MinTreeDepth = 1,
		MaxTreeDepth = 8,  // 2的8次方，为 256 X 256
	};

	void Create(AABB* _worldBox, int32 _depth);
	void Destroy();

	/// <summary>
	/// 增加或者更新 SceneObject
	/// 1.将物体BOX转换成四叉树空间
	/// 2.用异或特性找到四叉树层级
	/// 3.找到节点，添加物体
	/// </summary>
	/// <param name="newObject"></param>
	QuadTreeNode* AddOrUpdateSceneObject(SceneObject* newObject);
	QuadTreeNode* FindTreeNode(const QuadTreeRect& rect);

	void TestNodeObject();

	vector<SceneObject*> GetAllObject();
	// 视锥体裁剪
	vector<ISceneNode*> BuildSearchResults(
		const AABB& cameraRect,
		const Frustum* pOptionalFrustum,
		int32& nodeCount);

	QuadTreeNode* GetRootNode() { return &levelNodes[0][0]; }
	const Vector3& GetWorldOffset()const { return worldOffset; }
	const Vector3& GetWorldScale()const { return worldScale; }

private:
	QuadTreeNode* levelNodes[MaxTreeDepth];
	int32 depth;

	AABB*   worldBox;
	Vector3 worldOffset;
	Vector3 worldScale;

	QuadTreeNode* GetNodeFromLevelXY(int32 level, int32 x, int32 y);
};

#endif