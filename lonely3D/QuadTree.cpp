#include "QuadTree.h"


QuadTreeNode* QuadTree::GetNodeFromLevelXY(int32 level, int32 x, int32 y)
{
	if (level >= 0 && level < depth)
	{
		return &levelNodes[level][(y << level) + x];
	}
	return 0;
}


void QuadTree::Create(AABB* _worldBox, int32 _depth)
{
	Vector3 worldSize = _worldBox->GetSize();

	depth = _depth;
	worldBox = _worldBox;
	worldOffset = -_worldBox->min;
	worldScale.x = 256.0F / worldSize.x;
	worldScale.y = 256.0F / worldSize.y;
	worldScale.z = 256.0F / worldSize.z;

	Debug::LogInfo("�Ĳ����ָ�:");

	for (int32 i = 0;i < depth;i++)
	{
		int32 length = FLAG(i) * FLAG(i);
		levelNodes[i] = (QuadTreeNode*) new QuadTreeNode[length];	
	}

	static int32 totalLevelIndex = 0;

	for (int32 i = 0;i < depth;i++)
	{
		int32 nodeCount = FLAG(i);
		int32 levelIndex = 0;
		int32 totalCount = nodeCount * nodeCount;

		Debug::LogInfo("��[" + Convert(i) + "]��,"
			+ "�ڵ�[" + Convert(nodeCount) + "X" +
			Convert(nodeCount) + "=" + Convert(totalCount) + "]��");

		for (int32 y = 0;y < nodeCount;y++)
		{
			for (int32 x = 0;x < nodeCount;x++)
			{
				levelNodes[i][levelIndex].Setup(
					levelIndex, i, x, y,
					GetNodeFromLevelXY(i - 1, x >> 1, y >> 1),
					GetNodeFromLevelXY(i + 1, x << 1, y << 1),
					GetNodeFromLevelXY(i + 1, (x << 1) + 1, y << 1),
					GetNodeFromLevelXY(i + 1, (x << 1) + 1, (y << 1) + 1),
					GetNodeFromLevelXY(i + 1, x << 1, (y << 1) + 1));
				levelIndex++;
				totalLevelIndex++;
			}
		}
	}

	Debug::LogInfo("�ܽ��[" + Convert(totalLevelIndex) + "]");
	Debug::LogInfo("------------------------------------");
}

void QuadTree::Destroy()
{
	if (levelNodes)
	{
		for (int32 i = 0;i < depth;i++)
		{
			delete[] levelNodes[i];
		}
	}

	depth = 0;
	worldBox = 0;
}

// ���������󻮷ָ��Ĳ����Ľڵ�
QuadTreeNode* QuadTree::AddOrUpdateSceneObject(SceneObject* newObject)
{
	// ������ boxת���� �Ĳ���
	AABB* box = newObject->GetWorldBounds();
	QuadTreeRect rect;
	rect.Convert(*box, worldOffset, worldScale);

	QuadTreeNode* node = FindTreeNode(rect);

	if (node != 0)
	{
		node->SetQuadTreeRect(rect);
		uint32 level = node->GetLevel();
		int32 x = node->GetX();
		int32 y = node->GetY();

		QuadTreeNode* srcNode = newObject->GetQuadTreeNode();
		if (srcNode) srcNode->RemoveObject(newObject);

		node->AddObject(newObject);
		return node;
	}

	return 0;
}

QuadTreeNode* QuadTree::FindTreeNode(const QuadTreeRect& rect)
{
	// �ҵ��ڵ���� �Ĳ����ռ�
	// �����㣬�����ڵ�
	int32 orX = rect.x0 ^ rect.x1;
	int32 orY = rect.y0 ^ rect.y1;

	int32 orValue = Min(orX, orY);

	int32 maxBit = NearestPowerOfTwo(orValue);

	int32 currentLevel = MaxTreeDepth - maxBit;
	currentLevel = Min(depth - 1, currentLevel);

	int32 shift = MaxTreeDepth - currentLevel;
	int32 x = rect.x1 >> shift;
	int32 y = rect.y1 >> shift;

	return GetNodeFromLevelXY(currentLevel, x, y);
}

vector<SceneObject*> QuadTree::GetAllObject()
{
	vector<SceneObject*> allSceneNodes;

	int32 level = 0;

	while (level < depth)
	{
		int32 nodeCount = 1 << level;

		for (int32 y = 0;y < nodeCount;y++)
		{
			for (int32 x = 0;x < nodeCount;x++)
			{
				QuadTreeNode* treeNode = GetNodeFromLevelXY(level, x, y);
				vector<SceneObject*> nodeObjects = treeNode->GetAllNodes();

				if (nodeObjects.size() != 0)
				{
					QuadTreeRect rect = treeNode->GetRect();
					Debug::LogInfo("-----------------------------------------");
					Debug::LogInfo("[X0=" + Convert(rect.x0)
						+ ",Y0=" + Convert(rect.y0) + "]");
					Debug::LogInfo("[X1=" + Convert(rect.x1)
						+ ",Y1=" + Convert(rect.y1) + "]");

					for (uint32 i = 0;i < nodeObjects.size();i++)
					{
						Debug::LogInfo("����λ��=",
							nodeObjects[i]->GetLocalPosition());

						Debug::LogInfo("����=[" +
							Convert(nodeObjects[i]->GetNodeID())
							+ "]�����˵�[" + Convert(level) + "]��,X="
							+ Convert(x) + ",Y=" + Convert(y));
						allSceneNodes.push_back(nodeObjects[i]);
					}
				}
			}
		}

		level++;
	}

	return allSceneNodes;
}

vector<ISceneNode*> QuadTree::BuildSearchResults(
	const AABB& cameraRect, 
	const Frustum* pOptionalFrustum,
	int32& nodeCount)
{
	nodeCount = 0;
	// �������boxת�����Ĳ����ռ�
	QuadTreeRect rect;
	rect.Convert(cameraRect, worldOffset, worldScale);

	vector<ISceneNode*> allObjects;

	int32 level = 0;
	while (level < depth)
	{
		int32 shift = MaxTreeDepth - level;

		QuadTreeRect localRect(
			rect.x0 >> shift, rect.x1 >> shift,
			rect.y0 >> shift, rect.y1 >> shift,
			rect.z0 >> shift, rect.z1 >> shift);

		vector<ISceneNode*> childNode;

		for (int32 y = localRect.y0;y <= localRect.y1;y++)
		{
			for (int32 x = localRect.x0;x <= localRect.x1;x++)
			{
				QuadTreeNode* node = GetNodeFromLevelXY(level, x, y);
				childNode = node->SearchObject(pOptionalFrustum, nodeCount);

				if (childNode.size())
				{
					for (uint32 i = 0;i < childNode.size();i++)
					{
						allObjects.push_back(childNode[i]);
					}
				}
			}
		}

		level++;
	}

	return allObjects;
}

void QuadTree::TestNodeObject()
{
	Debug::LogInfo("---------------------------�Ĳ����ü�����--------------------------------");
	// ���� ����

	int32 totalNodeCount = 0;

	for (int32 i = 0;i < depth;i++)
	{
		int32 nodeCount = FLAG(i);
		int32 levelIndex = 0;

		for (int32 y = 0;y < nodeCount;y++)
		{
			for (int32 x = 0;x < nodeCount;x++)
			{
				levelNodes[i][levelIndex].TestNode(totalNodeCount);
				levelIndex++;
			}
		}
	}

	Debug::LogInfo("��������=" + Convert(totalNodeCount));

}