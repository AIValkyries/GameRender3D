#include "QuadTreeSceneNode.h"
#include "GameHost.h"

ISceneNode* QuadTreeSceneNode::Create(GameHost* _gameHost)
{
	SceneObject::Create(_gameHost);

	worldBounds = new AABB();
	worldBounds->Set(
		-500, -500, 1.0F,
		500, 500, 380);
	
	sourceBounds = new AABB();
	sourceBounds->Set(worldBounds);

	quadTree = new QuadTree();
	quadTree->Create(worldBounds, 3);

	MeshBuffer* mb = new MeshBuffer();
	mb->SetAABB(sourceBounds);

	meshBuffers.push_back(mb);

	// test
	gameHost->GetCamera()->Update(true);
	const AABB* cameraBox = gameHost->GetCamera()->GetAABB();

	Vector3 worldOffset = GetQuadTree()->GetWorldOffset();
	Vector3 worldScale = GetQuadTree()->GetWorldScale();

	QuadTreeRect rect;
	rect.Convert(*cameraBox, worldOffset, worldScale);

	Debug::LogInfo("------------------摄像机Box转换成四叉树空间-------------------------");
	Debug::LogInfo("MIN=", cameraBox->min);
	Debug::LogInfo("MAX=", cameraBox->max);

	Debug::LogInfo("[X0=" + Convert(rect.x0)
		+ ",Y0=" + Convert(rect.y0) + "]");
	Debug::LogInfo("[X1=" + Convert(rect.x1)
		+ ",Y1=" + Convert(rect.y1) + "]");
	Debug::LogInfo("------------------------------------------");

	int32 level = 0;
	while (level < 3)
	{
		int32 shift = 8 - level;

		QuadTreeRect localRect(
			rect.x0 >> shift, rect.x1 >> shift,
			rect.y0 >> shift, rect.y1 >> shift,
			rect.z0 >> shift, rect.z1 >> shift);

		Debug::LogInfo("------------------------------------------------");
		Debug::LogInfo("LEVEL=" + Convert(level) +
			",[X0=" + Convert(localRect.x0) +
			",X1=" + Convert(localRect.x1) + "]"+ 
			",[Y0=" + Convert(localRect.y0) +
			",Y1=" + Convert(localRect.y1) + "]");
		level++;
	}

	return this;
}

void QuadTreeSceneNode::Render(bool updateChild)
{
	SceneObject::Render(updateChild);
}


void QuadTreeSceneNode::UpdateBox(bool updateChild)
{
	if (worldBounds && sourceBounds)
	{
		worldBounds->Set(sourceBounds);
		TransformBox(localToWorldMatrix, *worldBounds);
	}

	SceneObject::UpdateBox(updateChild);
}


void QuadTreeSceneNode::Destroy()
{
	Debug::LogInfo("移除四叉树网格!!");

	for (uint32 i = 0;i < meshBuffers.size();i++)
	{
		meshBuffers[i]->Destroy();
		if (meshBuffers[i])
		{
			delete meshBuffers[i];
			meshBuffers[i] = 0;
		}
	}

	SafeDelete(worldBounds);
	sourceBounds = 0;
	meshBuffers.clear();
	quadTree->Destroy();
	quadTree = 0;

	SceneObject::Destroy();
}
