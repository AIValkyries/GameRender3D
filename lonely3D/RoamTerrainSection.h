#pragma once
#ifndef __RoamTerrainSection_H__
#define __RoamTerrainSection_H__

#include "UtilTools.h"
#include "TerrainSection.h"

/*           基本理论                  */
	// 1.主要思想:等边三角形分割
	// 2.分割决策 (ES/D)>L  E:误差(地形复杂度)
       //E:误差度量 S:误差缩放 D:视角距离 L:阈值
	//    构建误差方法:根据插值高度和真实高度的差值
	// 3.裂缝修补

	/*          实现思想:构建三角形二叉树           */
	//1.建立误差树
	//2.根据视角距离和地形复杂度,构建二叉树
	//3.裂缝修补:分割节点
    //4.构建三角形索引
	// 二叉树深度:假设地形为16*16=256，而二叉树与地形比例为1:1 则需要8层  2^8


class TriangleNode
{
public:
	// 这两个StringName是测试数据
	string Name;
	string DirName;
	uint32 NodeID;
	char Edge;

	TriangleNode* leftNeighbor;
	TriangleNode* rightNeighbor;
	TriangleNode* baseNeighbor;
	TriangleNode* leftChild;
	TriangleNode* rightChild;

	TriangleNode();
	~TriangleNode();
};

inline TriangleNode::TriangleNode() 
{
	leftNeighbor = 0;
	rightNeighbor = 0;
	baseNeighbor = 0;
	leftChild = 0;
	rightChild = 0;
}

inline TriangleNode::~TriangleNode()
{
	delete leftNeighbor;
	delete rightNeighbor;
	delete baseNeighbor;
	delete leftChild;
	delete rightChild;
}

// 设地形块为 16x16大小, 则三角形数量为 16*16*2=512
class RoamTerrainSystem;
class RoamTerrainSection:public TerrainSection
{
public:
	RoamTerrainSection();

	enum Constant
	{
		// 2^1 = 2
		// 2^2 = 4
		// 2^3 = 8
		// 2^4 = 16
		// 2^5 = 32 
		// 2^6 = 64
		// 2^7 = 128
		// 2^8 = 256
		//TOTAL_DEPTH = 8,                    // 二叉树深度
		//TOTAL_VARIANCES = 1 << TOTAL_DEPTH,            // 总节点 512
		TOTAL_TRITREENODES = 1024 * 16
	};

	virtual void PreUpdate();
	virtual void Update(bool updateChild);
	virtual void Destroy();
	virtual void Render(bool updateChild);

	virtual void Create(
		Terrain* terrain,
		int32 _sectorX, int32 _sectorY,
		int32 _heightMapX, int32 _heightMapY,
		int32 _vertexCount, Vector2 _sectorSize,
		uint32 _chunkSizeShift,
		GameHost* _gameHost);

	TriangleNode* GetTreeA() { return &treeA; }
	TriangleNode* GetTreeB() { return &treeB; }

	void Reset();

	void SetForceSplit(bool forceSplit)
	{
		isForceSplit = forceSplit;
	}

protected:
	virtual void BuildIndexBuffer();

private:
	RoamTerrainSystem* roamTerrain;

	float32 distance0;
	float32 distance1;
	float32 distance2;
	float32 distance3;

	TriangleNode treeA;
	TriangleNode treeB;

	bool initErrorTree;
	bool isForceSplit;

	float32* errorTreeA;
	float32* errorTreeB;

	TriangleNode* leftNeighborOfA;
	TriangleNode* rightNeighborOfA;
	TriangleNode* leftNeighborOfB;
	TriangleNode* rightNeighborOfB;

	uint32 totalIndices;
	uint32 maxIndices;

	void InitTriTreeAB();

	/// <summary>
	/// 3.构建三角形索引
	/// </summary>
	void BuildTriangleList();
	void RecursiveBuildTriangleList(
		TriangleNode* node,
		uint32 vertex0, uint32 vertex1, uint32 vertex2);

	/// <summary>
	/// 2.构建二叉树
	/// </summary>
	void BuildTriangleBinaryTree();
	void RecursiveBuildTriangleBinaryTree(
		TriangleNode* tree, float32* errorTree, int32 nodeIndex,
		float32 distance0, float32 distance1, float32 distance2);
	void Split(TriangleNode* tree);
	
	/// <summary>
	/// 1.构建误差树
	/// </summary>
	void BuildErrorTree();
	float32 RecursionBuildErrorTree(
		float32* tree,int32 nodeIndex,
		int32 index0,int32 index1,int32 index2,
		float32 height0,float32 height1,float32 height2);
};


inline RoamTerrainSection::RoamTerrainSection()
{
	initErrorTree = false;
}


#endif

