#include "RoamTerrainSection.h"
#include "RoamTerrainSystem.h"
#include "GameHost.h"

void RoamTerrainSection::Create(
	Terrain* terrain,
	int32 _sectorX, int32 _sectorY,
	int32 _heightMapX, int32 _heightMapY,
	int32 _vertexCount, Vector2 _sectorSize,
	uint32 _chunkSizeShift,
	GameHost* _gameHost)
{
	TerrainSection::Create(
		terrain, _sectorX, _sectorY,
		_heightMapX, _heightMapY,
		_vertexCount, _sectorSize,
		_chunkSizeShift, _gameHost);

	roamTerrain = (RoamTerrainSystem*)terrain;
	maxIndices = _vertexCount * _vertexCount * 2 * 3;
	totalIndices = 0;
	isForceSplit = false;

	errorTreeA = new float32[terrain->GetTotalVariances()];
	errorTreeB = new float32[terrain->GetTotalVariances()];
	memset(errorTreeA, 0, sizeof(float32) * terrain->GetTotalVariances());
	memset(errorTreeB, 0, sizeof(float32) * terrain->GetTotalVariances());

	InitTriTreeAB();
	BuildErrorTree();
}

void RoamTerrainSection::BuildIndexBuffer()
{
	// 空的
}

//          北
/*   2------------------3
     |        |         |
     |6(0,1)  |  7(1,1) |
	 |--------|---------|
 西  |        |         |  东
     | 4(0,0) |  5(1,0) |
     0--------|---------1
            南

			2-1为分割线，不会画线
			TreeA为 0-1-2
			TreeB为 3-2-1
*/
void RoamTerrainSection::InitTriTreeAB()
{
	leftNeighborOfA = 0;
	rightNeighborOfA = 0;
	leftNeighborOfB = 0;
	rightNeighborOfB = 0;

	/*        North
			   北

	  West               East
	  西                 东


			    南
			  South
    */

	//Debug::LogError("NODE_ID=" + Convert(GetNodeID()) +
	//	",[X=" + Convert(sectorX) + ",Y=" + Convert(sectorY) + "]");

	treeA.Name = "[" + Convert(GetNodeID()) + ".A]";
	treeB.Name = "[" + Convert(GetNodeID()) + ".B]";
	treeA.NodeID = GetNodeID();
	treeB.NodeID = GetNodeID();
	treeA.Edge = 'A';
	treeB.Edge = 'B';

	RoamTerrainSection* pNorth = 
		roamTerrain->FindSection(uint16(sectorX), uint16(sectorY + 1));
	RoamTerrainSection* pSouth =
		roamTerrain->FindSection(uint16(sectorX), uint16(sectorY - 1));

	RoamTerrainSection* pEast =
		roamTerrain->FindSection(uint16(sectorX + 1), uint16(sectorY));
	RoamTerrainSection* pWest = 
		roamTerrain->FindSection(uint16(sectorX - 1), uint16(sectorY));

	// TreeA = 0->16->272
	// TreeB = 288->272->16

	//if (GetNodeID() == 6)
	//{
	//	if (pWest)
	//	{
	//		Debug::LogError("西=" + Convert(pWest->GetNodeID()));
	//	}
	//	if (pSouth)
	//	{
	//		Debug::LogError("南=" + Convert(pSouth->GetNodeID()));
	//	}

	//	if (pEast)
	//	{
	//		Debug::LogError("东=" + Convert(pEast->GetNodeID()));
	//	}

	//	if (pNorth)
	//	{
	//		Debug::LogError("北=" + Convert(pNorth->GetNodeID()));
	//	}

	//	int32 count = 1;
	//}

	if (pNorth) //北
	{
		leftNeighborOfB = pNorth->GetTreeA();
		leftNeighborOfB->DirName = "北";
	}
	if (pEast) //东
	{
		rightNeighborOfB = pEast->GetTreeA();
		rightNeighborOfB->DirName = "东";
	}

	if (pSouth)  //南
	{
		leftNeighborOfA = pSouth->GetTreeB();
		leftNeighborOfA->DirName = "南";
	}
	if (pWest)  // 西
	{
		rightNeighborOfA = pWest->GetTreeB();
		rightNeighborOfA->DirName = "西";
	}

	Reset();	
}

#pragma region 构建三角形索引

/* 272------------------288
      |                  |
      |                  |
      |                  |
      |                  |
     0 ------------------16
*/

static int32 triangleCount = 0;

void RoamTerrainSection::BuildTriangleList()
{
	uint32 verts = (1 << chunkSizeShift) + 1;

	uint32 c0 = 0;
	uint32 c1 = verts * (verts - 1);
	uint32 c2 = (verts * verts) - 1;
	uint32 c3 = verts - 1;

	totalIndices = 0;
	buffer->ClearIndices();
	triangleCount = 0;

	// 0->16->272
	RecursiveBuildTriangleList(&treeA, c0, c3, c1);

	//Debug::LogError("----------------------------------------");
	//Debug::LogError(totalIndices);

	// 288->272->16
	RecursiveBuildTriangleList(&treeB, c2, c1, c3);

	//Debug::LogError("面=" + Convert(triangleCount));
	//int32 count = 1;

}

void RoamTerrainSection::RecursiveBuildTriangleList(
	TriangleNode* node,
	uint32 vertex0, uint32 vertex1, uint32 vertex2)
{
	if (node->leftChild)
	{
		uint32 interVertex = (vertex1 + vertex2) >> 1;

		//Debug::LogError("--------------------------------");
		//Debug::LogError(
		//	"vertex0=" + Convert(interVertex) +
		//	",vertex1=" + Convert(vertex2) +
		//	",vertex2=" + Convert(vertex0));

		RecursiveBuildTriangleList(node->leftChild,
			interVertex, vertex0, vertex1);

		RecursiveBuildTriangleList(node->rightChild,
			interVertex, vertex2, vertex0);
	}
	else if ((totalIndices + 3) < maxIndices)
	{
		triangleCount++;
		//Debug::LogError("----------------"+Convert(triangleCount)+"-----------------------");
		//Debug::LogError(
		//	"vertex0=" + Convert(vertex0) + 
		//	",vertex1=" + Convert(vertex1) + 
		//	",vertex2=" + Convert(vertex2));

		//totalIndices++;
		//Debug::LogError(
		//	"totalIndices0=" + Convert(totalIndices));

		//totalIndices++;
		//Debug::LogError(
		//	"totalIndices1=" + Convert(totalIndices));

		//totalIndices++;
		//Debug::LogError(
		//	"totalIndices2=" + Convert(totalIndices));

		totalIndices += 3;
		buffer->PushIndices(vertex2);
		buffer->PushIndices(vertex1);
		buffer->PushIndices(vertex0);
		
	}
}

#pragma endregion

#pragma region 构建二叉树

static uint32 binaryTreeCount = 0;

// 根据视角距离和地形复杂度进行构建二叉树
void RoamTerrainSection::BuildTriangleBinaryTree()
{
	//uint32 verts = (1 << chunkSizeShift) + 1;
	//uint32 c0 = 0;
	//uint32 c1 = verts * (verts - 1);
	//uint32 c2 = (verts * verts) - 1;
	//uint32 c3 = verts - 1;

	// 0->16->272
	RecursiveBuildTriangleBinaryTree(
		&treeA, errorTreeA, 1,
		distance1, distance2, distance0);

	//Debug::LogError("左边二叉树节点数量=" + Convert(binaryTreeCount));
	binaryTreeCount = 0;

	// 288->272->16
	RecursiveBuildTriangleBinaryTree(
		&treeB, errorTreeB, 1,
		distance3, distance0, distance2);

	//Debug::LogError("右边二叉树节点数量=" + Convert(binaryTreeCount));
}

//  分割决策 ES/D>L, S:误差缩放,D:视角距离, L:阈值
void RoamTerrainSection::RecursiveBuildTriangleBinaryTree(
	TriangleNode* tree, float32* errorTree, int32 nodeIndex,
	float32 distance0, float32 distance1, float32 distance2)
{
	if ((uint32)(nodeIndex << 1) <= mainTerrain->GetTotalVariances())
	{
		binaryTreeCount++;

		float32 erroScale = roamTerrain->GetES();
		float32 ratioLimit = roamTerrain->GetRL();

		float32 es = errorTree[nodeIndex] * erroScale;

		float32 midDist = (distance1 + distance2) * 0.5F;
		//Debug::LogError("索引=" + Convert(interIndex));

		if (!tree->leftChild)
		{
			// midDist  越大 ratio 越小
			float32 ratio = es / (midDist + 0.0001F);
			
			if (isForceSplit)
			{
				Split(tree);
			}
			else
			{
				if (ratio > ratioLimit)
					Split(tree);
			}
		}

		if (tree->leftChild)
		{
			RecursiveBuildTriangleBinaryTree(
				tree->leftChild, errorTree, (nodeIndex << 1),
				midDist, distance0, distance1);

			RecursiveBuildTriangleBinaryTree(
				tree->rightChild, errorTree, (nodeIndex << 1) + 1,
				midDist, distance2, distance0);
		}

	}
}

// 三角形分割
void RoamTerrainSection::Split(TriangleNode* node)
{
	if (node->leftChild)
		return;

	if (node->baseNeighbor && (node->baseNeighbor->baseNeighbor != node))
		Split(node->baseNeighbor);

	node->leftChild = roamTerrain->CreateNode();
	node->rightChild = roamTerrain->CreateNode();

	node->leftChild->Name = Convert(node->NodeID) + "." + node->Edge + ".LEFT";
	node->rightChild->Name = Convert(node->NodeID) + "." + node->Edge + ".RIGHT";
	node->leftChild->Edge = node->Edge;
	node->rightChild->Edge = node->Edge;
	node->leftChild->NodeID = node->NodeID;
	node->rightChild->NodeID = node->NodeID;

	if (!node->leftChild || !node->rightChild)
	{
		node->leftChild  = 0;
		node->rightChild = 0;
		return;
	}

	node->leftChild->baseNeighbor = node->leftNeighbor;
	node->leftChild->leftNeighbor = node->rightChild;

	node->rightChild->baseNeighbor = node->rightNeighbor;
	node->rightChild->rightNeighbor = node->leftChild;

	if (node->leftNeighbor != NULL)
	{
		if (node->leftNeighbor->baseNeighbor == node)
		{
			node->leftNeighbor->baseNeighbor = node->leftChild;
		}
		else if (node->leftNeighbor->leftNeighbor == node)
		{
			node->leftNeighbor->leftNeighbor = node->leftChild;
		}
		else if (node->leftNeighbor->rightNeighbor == node)
		{
			node->leftNeighbor->rightNeighbor = node->leftChild;
		}
		else
		{
			Debug::LogError("leftNeighbor ERROR");
		}
	}

	if (node->rightNeighbor != NULL)
	{
		if (node->rightNeighbor->baseNeighbor == node)
		{
			node->rightNeighbor->baseNeighbor = node->rightChild;
		}
		else if (node->rightNeighbor->leftNeighbor == node)
		{
			node->rightNeighbor->leftNeighbor = node->rightChild;
		}
		else if (node->rightNeighbor->rightNeighbor == node)
		{
			node->rightNeighbor->rightNeighbor = node->rightChild;
		}
		else
		{
			Debug::LogError("rightNeighbor ERROR");
		}
	}

	if (node->baseNeighbor != NULL)
	{
		if (node->baseNeighbor->leftChild != NULL)
		{
			node->leftChild->rightNeighbor = node->baseNeighbor->rightChild;
			node->rightChild->leftNeighbor = node->baseNeighbor->leftChild;

			node->baseNeighbor->leftChild->rightNeighbor = node->rightChild;
			node->baseNeighbor->rightChild->leftNeighbor = node->leftChild;
		}
		else
		{
			Split(node->baseNeighbor);
		}
	}
	else
	{
		node->leftChild->rightNeighbor = 0;
		node->rightChild->leftNeighbor = 0;
	}

}

#pragma endregion

#pragma region 构建误差三角树

/* 0------------------3 
   |                  |
   |                  |
   |                  |
   |                  |
   1------------------2 
*/

static int32 errorTreeCount = 0;

void RoamTerrainSection::BuildErrorTree()
{
	uint32 mapIndex0;
	uint32 mapIndex1;
	uint32 mapIndex2;
	uint32 mapIndex3;

	uint32 x0, y0, x1, y1;

	ExtractFourCornersIndex(
		mapIndex0, mapIndex1, mapIndex2, mapIndex3,
		x0, y0, x1, y1);

	Debug::LogInfo("---------------------地形块的四个角信息---------------------------");
	Debug::LogInfo(
		" INDEX_0=" + Convert(mapIndex0) + ",INDEX_1=" + Convert(mapIndex1) +
		",INDEX_2=" + Convert(mapIndex2) + ",INDEX_3=" + Convert(mapIndex3));

	Debug::LogInfo(
		" X0=" + Convert(x0) + ",X1=" + Convert(x1) +
		",Y0=" + Convert(y0) + ",Y1=" + Convert(y1));

	float32 h0 = mainTerrain->ReadHeight(mapIndex0);
	float32 h1 = mainTerrain->ReadHeight(mapIndex1);
	float32 h2 = mainTerrain->ReadHeight(mapIndex2);
	float32 h3 = mainTerrain->ReadHeight(mapIndex3);

	Debug::LogInfo(
		" H_0=" + Convert(h0) + ",H_1=" + Convert(h1) +
		",H_2=" + Convert(h2) + ",H_3=" + Convert(h3));

	errorTreeCount = 0;

	RecursionBuildErrorTree(
		errorTreeA, 1, 
		mapIndex1, mapIndex2, mapIndex0,
		h1, h2, h0);

	Debug::LogInfo("左边树节点=" + Convert(errorTreeCount));

	errorTreeCount = 0;

	RecursionBuildErrorTree(
		errorTreeB, 1,
		mapIndex3, mapIndex0, mapIndex2,
		h3, h0, h2);

	Debug::LogInfo("右边树节点=" + Convert(errorTreeCount));

	initErrorTree = true;
}

// 真实地形高度和插值地形高度进行比较
// 自下而上构建数组
float32 RoamTerrainSection::RecursionBuildErrorTree(
	float32* tree, int32 nodeIndex,
	int32 index0, int32 index1, int32 index2,
	float32 height0, float32 height1, float32 height2)
{
	if (nodeIndex < (int32)mainTerrain->GetTotalVariances())
	{
		errorTreeCount++;
		
		// 高度插值与索引插值的高度对比
		float32 interHeight = (height1 + height2) * 0.5F;
		int32 interIndex = int32((index1 + index2) * 0.5F);
		float32 realHeight =  mainTerrain->ReadHeight(interIndex);

		float32 error = AbsF(realHeight - interHeight);

		float32 leftE =
			RecursionBuildErrorTree(
				tree, nodeIndex << 1, 
				interIndex, index0, index1,
				realHeight, height0, height1);

		float32 rightE = RecursionBuildErrorTree(
			tree, (nodeIndex << 1) + 1,
			interIndex, index2, index0,
			realHeight, height2, height0);

		float32 maxError = Max(leftE, rightE);

	//	Debug::LogError(
	//"errorTreeCount=" + Convert(errorTreeCount) +
	//":maxError=" + Convert(maxError)+",nodeIndex="+Convert(nodeIndex));

		tree[nodeIndex] = maxError;

		return error;
	}

	return 0;
}

#pragma endregion

void RoamTerrainSection::Render(bool updateChild)
{
	if (GetNodeID() == 4 || GetNodeID() == 6)
	{
		
	}

	SceneObject::Render(updateChild);
}

void RoamTerrainSection::PreUpdate()
{
	// 计算地形与摄像机的距离
	Vector4 cv4 = gameHost->GetCamera()->GetLocalPosition();

	Vector2 cameraPos(cv4.x, cv4.y);
	Vector2 corner0(sourceBounds->min.x, sourceBounds->min.z);
	Vector2 corner1(sourceBounds->min.x, sourceBounds->max.z);
	Vector2 corner2(sourceBounds->max.x, sourceBounds->max.z);
	Vector2 corner3(sourceBounds->max.x, sourceBounds->min.z);

	////Debug::LogError("---------------------------------------------");
	distance0 = Distance(cameraPos, corner0);
	distance1 = Distance(cameraPos, corner1);
	distance2 = Distance(cameraPos, corner2);
	distance3 = Distance(cameraPos, corner3);

	// 最小距离
	queueSortValue = Min(distance0, distance1);
	queueSortValue = Min(queueSortValue, distance2);
	queueSortValue = Min(queueSortValue, distance3);

	SceneObject::PreUpdate();
}

void RoamTerrainSection::Update(bool updateChild)
{
	//Debug::LogError(Convert(GetNodeID()));
	if (GetNodeID() == 11)
	{
		int32 c = 1;
		//isForceSplit = true;
		//totalVariances = 6;
	}

	if (GetNodeID() == 5)
	{
		int32 c = 1;
	}

	if (GetNodeID() == 6)
	{
		int32 c = 1;

	}

	if (GetNodeID() == 7)
	{
		int32 c = 1;
	}

	TerrainSection::Update(updateChild);
	if (initErrorTree)
	{
		BuildTriangleBinaryTree();   // 根据误差二叉树构建二叉树
		BuildTriangleList();
	}
}

void RoamTerrainSection::Reset()
{
	//Debug::LogError(
	//	"Reset=" + Convert(GetNodeID()) + 
	//	",A_Name=" + treeA.Name + 
	//	",B_Name=" + treeB.Name);
	treeA.leftChild = 0;
	treeA.rightChild = 0;
	treeB.leftChild = 0;
	treeB.rightChild = 0;

	treeA.baseNeighbor = &treeB;
	treeB.baseNeighbor = &treeA;

	treeA.leftNeighbor = leftNeighborOfA;
	treeA.rightNeighbor = rightNeighborOfA;

	treeB.leftNeighbor = leftNeighborOfB;
	treeB.rightNeighbor = rightNeighborOfB;
}

void RoamTerrainSection::Destroy()
{
	SafeDelete(errorTreeA);
	SafeDelete(errorTreeB);
	SafeDelete(leftNeighborOfA);
	SafeDelete(rightNeighborOfA);
	SafeDelete(leftNeighborOfB);
	SafeDelete(rightNeighborOfB);

	TerrainSection::Destroy();
}


