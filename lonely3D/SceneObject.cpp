#include "SceneObject.h"
#include "QuadTreeNode.h"
#include "QuadTree.h"
#include "GameHost.h"

ISceneNode* SceneObject::Create(GameHost* _gameHost)
{
	gameHost = _gameHost;

	eulerAngle = Vector3::zero();
	SetRotate(Quaternion::identity());
	SetScale(Vector3::one());
	SetPosition(Vector4::zero());

	globaLocalToWorldMatrix.SetIdentity();
	globaWorldToLocalMatrix.SetIdentity();
	localToWorldMatrix.SetIdentity();
	worldToLocalMatrix.SetIdentity();

	parent       = 0;
	node         = 0;
	quadTree     = 0;
	worldBounds  = 0;
	sourceBounds = 0;
	queueSortValue = 0;

	SetVisible(true);

	return this;
}

void SceneObject::Render(bool updateChild)
{
	if (!isVisible)
		return;

	if (meshBuffers.size())
	{
		for (uint32 i = 0;i < meshBuffers.size();i++)
		{ 
			if (meshBuffers[i]->GetIndexBuffer() == 0)
				continue;

			// 法线变换矩阵
			Matrix4x4 temp = globaLocalToWorldMatrix.Inverse();
			Matrix4x4 normalMat = globaWorldToLocalMatrix;
			normalMat.Transpose();
			gameHost->Model_2_World(meshBuffers[i], globaLocalToWorldMatrix, normalMat);

			lMaterial* m = meshBuffers[i]->GetMaterial();

			bool enableLight = false;

			if (meshBuffers[i]->CheckRenderState(SPECULAR_LIGHT) ||
				meshBuffers[i]->CheckRenderState(AMBIENT_LIGHT) ||
				meshBuffers[i]->CheckRenderState(DIFFUSE_LIGHT))
			{
				// 光照
				gameHost->GetLightSpace()->ApplyLight(meshBuffers[i]);
				enableLight = true;
			}
		
			gameHost->World_2_View(meshBuffers[i]);
			gameHost->DrawVertexPrimitiveList(
				meshBuffers[i],
				globaLocalToWorldMatrix,
				textureMulColor,
				GetShaderType());

			meshBuffers[i]->ResetVertexCount();
		}
	}

	if (updateChild)
	{
		vector<ISceneNode*>::iterator itr;
		for (itr = children.begin();itr != children.end();itr++)
		{
			(*itr)->Render(updateChild);
		}
	}

}


void SceneObject::RenderBox(bool updateChild)
{
	if (!isVisible)
		return;

	if (meshBuffers.size())
	{
		for (uint32 i = 0;i < meshBuffers.size();i++)
		{
			if (meshBuffers[i]->CheckRenderState(RENDER_BOX))
			{
				gameHost->DrawBox(
					meshBuffers[i], globaLocalToWorldMatrix,GetShaderType());
			}
		}
	}

	if (updateChild)
	{
		vector<ISceneNode*>::iterator itr;
		for (itr = children.begin();itr != children.end();itr++)
		{
			(*itr)->RenderBox(updateChild);
		}
	}

}

void SceneObject::PreUpdate()
{
	if (children.size())
	{
		vector<ISceneNode*>::iterator itr;
		for (itr = children.begin();itr != children.end();itr++)
		{
			(*itr)->PreUpdate();
		}
	}
}

void SceneObject::Update(bool updateChild)
{
	UpdateMatrix();

	if (IsCanUpdateMatrix())
	{
		RefreshQuadTreeMembership();  // 重新计算在四叉树空间中的位置
	}

	if (updateChild)
	{
		vector<ISceneNode*>::iterator itr;
		for (itr = children.begin();itr != children.end();itr++)
		{
			(*itr)->Update(updateChild);
		}
	}

	updateFlags.ClearBitAll();
}

void SceneObject::UpdateBox(bool updateChild)
{
	if (children.size())
	{
		vector<ISceneNode*>::iterator itr;
		for (itr = children.begin();itr != children.end();itr++)
		{
			(*itr)->UpdateBox(updateChild);
		}
	}
}

void SceneObject::AttachToQuadTree(QuadTree* tree)
{
	UpdateBox(true);
	DetachFromQuadTree();
	quadTree = tree;
	node = quadTree->AddOrUpdateSceneObject(this);
}

void SceneObject::DetachFromQuadTree()
{
	if (node != 0)
	{
		node->RemoveObject(this);
		node = 0;
	}
	quadTree = 0;
}

// 如果物体的位置，缩放，旋转发生了改变，
// 则重新计算在四叉树中的位置
void SceneObject::RefreshQuadTreeMembership()
{
	if (quadTree != 0)
	{
		node = quadTree->AddOrUpdateSceneObject(this);
	}
}

// 重新计算 aabb,没用了
void SceneObject::RecalcWorldBounds()
{
	// TODO
}

void SceneObject::Destroy()
{
	for (uint32 i = 0;i < children.size();i++)
	{
		children[i]->Destroy();
		children[i] = 0;
	}
	children.clear();
	sibling.clear();
	parent = 0;

	if (meshBuffers.size())
	{
		for (uint32 i = 0;i < meshBuffers.size();i++)
		{
			meshBuffers[i] = 0;
		}
		meshBuffers.clear();
	}
	node = 0;
	quadTree = 0;
}