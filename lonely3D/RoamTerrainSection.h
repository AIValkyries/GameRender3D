#pragma once
#ifndef __RoamTerrainSection_H__
#define __RoamTerrainSection_H__

#include "UtilTools.h"
#include "TerrainSection.h"

/*           ��������                  */
	// 1.��Ҫ˼��:�ȱ������ηָ�
	// 2.�ָ���� (ES/D)>L  E:���(���θ��Ӷ�)
       //E:������ S:������� D:�ӽǾ��� L:��ֵ
	//    ��������:���ݲ�ֵ�߶Ⱥ���ʵ�߶ȵĲ�ֵ
	// 3.�ѷ��޲�

	/*          ʵ��˼��:���������ζ�����           */
	//1.���������
	//2.�����ӽǾ���͵��θ��Ӷ�,����������
	//3.�ѷ��޲�:�ָ�ڵ�
    //4.��������������
	// ���������:�������Ϊ16*16=256��������������α���Ϊ1:1 ����Ҫ8��  2^8


class TriangleNode
{
public:
	// ������StringName�ǲ�������
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

// ����ο�Ϊ 16x16��С, ������������Ϊ 16*16*2=512
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
		//TOTAL_DEPTH = 8,                    // ���������
		//TOTAL_VARIANCES = 1 << TOTAL_DEPTH,            // �ܽڵ� 512
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
	/// 3.��������������
	/// </summary>
	void BuildTriangleList();
	void RecursiveBuildTriangleList(
		TriangleNode* node,
		uint32 vertex0, uint32 vertex1, uint32 vertex2);

	/// <summary>
	/// 2.����������
	/// </summary>
	void BuildTriangleBinaryTree();
	void RecursiveBuildTriangleBinaryTree(
		TriangleNode* tree, float32* errorTree, int32 nodeIndex,
		float32 distance0, float32 distance1, float32 distance2);
	void Split(TriangleNode* tree);
	
	/// <summary>
	/// 1.���������
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

