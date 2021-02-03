#include "QuadTreeNode.h"
#include "Debug.h"

void QuadTreeNode::Setup(
	int32 _index,
	int32 _level,
	int32 _x,
	int32 _y,
	QuadTreeNode* _parent,
	QuadTreeNode* child0,
	QuadTreeNode* child1,
	QuadTreeNode* child2,
	QuadTreeNode* child3)
{
	x = _x;
	y = _y;
	level = _level;
	index = _index;
	parent = _parent;

	childs[0] = child0;
	childs[1] = child1;
	childs[2] = child2;
	childs[3] = child3;
}

bool QuadTreeNode::HasObject(SceneObject* sceneNode)
{
	for (uint32 i = 0;i < allNodes.size();i++)
	{
		if (allNodes[i]->GetNodeID() == sceneNode->GetNodeID())
		{
			return true;
		}
	}
	return false;
}

void QuadTreeNode::AddObject(SceneObject* sceneNode)
{
	allNodes.push_back(sceneNode);
}


void QuadTreeNode::RemoveObject(SceneObject* sceneNode)
{
	std::vector<SceneObject*>::iterator itr;
	for (itr = allNodes.begin();itr != allNodes.end();itr++)
	{
		if ((*itr) == sceneNode)
		{
			allNodes.erase(itr);
			break;
		}
	}
}

vector<ISceneNode*> QuadTreeNode::SearchObject(
	const Frustum* pOptionalFrustum, int32& nodeCount)
{
	vector<ISceneNode*> outNodes;
	nodeCount += allNodes.size();

	for (uint32 i = 0;i < allNodes.size();i++)
	{
		//Debug::LogInfo("-----------------------------------------------");

		AABB* box = allNodes[i]->GetWorldBounds();

		//Debug::LogInfo("NODE_ID="+Convert(allNodes[i]->GetNodeID()));
		//Debug::LogInfo("POS=", allNodes[i]->GetLocalPosition());
		//Debug::LogInfo("BOX_MIN=", box->min);
		//Debug::LogInfo("BOX_MAX=", box->max);
		//Debug::LogInfo("被视锥体包含=" + Convert((cTest ? 1 : 0)));

		if (pOptionalFrustum->TestAABB(*box))
		{
			outNodes.push_back(allNodes[i]);
		}
	}

	return outNodes;
}

void QuadTreeNode::TestNode(int32& totalCount)
{
	if (allNodes.size())
	{
		Debug::LogInfo("LEVEL=" + Convert(level) + ",X=" + Convert(x) + ",Y=" + Convert(y));
		Debug::LogInfo("OBJECT_SIZE=" + Convert(allNodes.size()));
		Debug::LogInfo("------------------------------------------------------");
		totalCount += allNodes.size();
	}
}





