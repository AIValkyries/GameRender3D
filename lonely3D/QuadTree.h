#pragma once
#ifndef __QuadTree_H__
#define __QuadTree_H__

#include "QuadTreeNode.h"
#include <vector>

/// <summary>
/// �㼶  �ָ�ƽ����  ����Ķ�����ֵ  ��߱���λ
//   0       128          10000000          7
//   1       64           01000000          6
//   2       32           00100000          5
//   3       16           00010000          4
//   4       8            00001000          3
//   5       4            00000100          2
//   6       2            00000010          1
//   7       1            00000001          0
// ���Ĳ����涨Ϊ  256 X 256 ����Ϊ 2^8  ���Ĳ�����ż���ָ�
// 1.������BOXת�����Ĳ����ռ� ���Ϊ256 X 256
// 2. r0 = x0^x1 ,r1 = y0^y1, r = Max(r0,r1)  �������
// 3. maxLevel - r = currentLevel
/// </summary>
class QuadTree
{
public:

	enum Constants
	{
		MinTreeDepth = 1,
		MaxTreeDepth = 8,  // 2��8�η���Ϊ 256 X 256
	};

	void Create(AABB* _worldBox, int32 _depth);
	void Destroy();

	/// <summary>
	/// ���ӻ��߸��� SceneObject
	/// 1.������BOXת�����Ĳ����ռ�
	/// 2.����������ҵ��Ĳ����㼶
	/// 3.�ҵ��ڵ㣬�������
	/// </summary>
	/// <param name="newObject"></param>
	QuadTreeNode* AddOrUpdateSceneObject(SceneObject* newObject);
	QuadTreeNode* FindTreeNode(const QuadTreeRect& rect);

	void TestNodeObject();

	vector<SceneObject*> GetAllObject();
	// ��׶��ü�
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