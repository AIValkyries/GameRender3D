#include "SkyBoxSceneNode.h"
#include "GameHost.h"

void SkyBoxSceneNode::CreateSkyBox(
	Texture* top,
	Texture* bottom,
	Texture* left,
	Texture* right,
	Texture* front,
	Texture* back)
{
	// 规范化体的8个顶点
	/* 上下
   (-1,1,1)     (1,1,1)
	  /7--------/6
	 /  |      / |
	/   |     /  |
   4----------5  |
	|   1- - -| -2
	|  /      |  /
	|/        | /
	0---------3/
(-1,-1,-1)

前后
   (-1,1,1)     (1,1,1)
	  /18--------/19
	 /  |       /|
	/   |      / |
   11---------10 |
	|   17-  -| -16
	|  /      |  /
	|/        | /
	8---------9/
(-1,-1,-1)

左右
   (-1,1,1)     (1,1,1)
	  /23--------/14
	 /  |       / |
	/   |      /  |
   22---------15  |
	|   20- - | -13
	|  /      |  /
	|/        | /
	21-------12/
(-1,-1,-1)

	*/
	Camera* camera = gameHost->GetCamera();

	static float32 edge = 1;

	// bottom
	{
		MeshBuffer* bottomBuffer = new MeshBuffer();

		Vertex* bottomV = new Vertex[4];
		bottomV[0] = Vertex(-edge, -edge, -edge,   0, -1, 0,    1, 1);
		bottomV[1] = Vertex(-edge, -edge, edge,    0, -1, 0,    1, 0);
		bottomV[2] = Vertex(edge, -edge, edge,     0, -1, 0,    0, 0);
		bottomV[3] = Vertex(edge, -edge, -edge,    0, -1, 0,    0, 1);

		bottomBuffer->CreateVertex(4);
		bottomBuffer->CreateIndices(INDEX_16BIT, 6);
		bottomBuffer->SetIndices(0, 0);
		bottomBuffer->SetIndices(1, 1);
		bottomBuffer->SetIndices(2, 2);
		bottomBuffer->SetIndices(3, 2);
		bottomBuffer->SetIndices(4, 3);
		bottomBuffer->SetIndices(5, 0);

		for (int32 i = 0;i < 4;i++)
		{
			bottomBuffer->SetSourceVertex(i, bottomV[i]);
			bottomBuffer->SetTransformVertex(i, bottomV[i]);
		}

		bottomBuffer->SetTexture(bottom);
		bottomBuffer->SetRenderState(RENDER_TRIANGLE, true);
		bottomBuffer->SetRenderState(Z_BUFFER, true);

		meshBuffers.push_back(bottomBuffer);
	}

	// top
	{
		MeshBuffer* topBuffer = new MeshBuffer();

		Vertex* topV = new Vertex[4];
		topV[0] = Vertex(-edge, edge, -edge,   0, 1, 0, 0, 0);
		topV[1] = Vertex(edge, edge, -edge,    0, 1, 0, 1, 0);
		topV[2] = Vertex(edge, edge, edge,     0, 1, 0, 1, 1);
		topV[3] = Vertex(-edge, edge, edge,    0, 1, 0, 0, 1);

		topBuffer->CreateVertex(4);
		topBuffer->CreateIndices(INDEX_16BIT, 6);

		topBuffer->SetIndices(0, 0);
		topBuffer->SetIndices(1, 1);
		topBuffer->SetIndices(2, 2);
		topBuffer->SetIndices(3, 2);
		topBuffer->SetIndices(4, 3);
		topBuffer->SetIndices(5, 0);

		for (int32 i = 0;i < 4;i++)
		{
			topBuffer->SetSourceVertex(i, topV[i]);
			topBuffer->SetTransformVertex(i, topV[i]);
		}

		topBuffer->SetTexture(top);
		topBuffer->SetRenderState(RENDER_TRIANGLE, true);
		topBuffer->SetRenderState(Z_BUFFER, true);

		meshBuffers.push_back(topBuffer);
	}

	// front
	{
		MeshBuffer* frontBuffer = new MeshBuffer();
		Vertex* frontV = new Vertex[4];
		frontV[0] = Vertex(-edge, -edge, -edge,   0, 0, 1,  0, 1);
		frontV[1] = Vertex(edge, -edge, -edge,    0, 0, 1,  1, 1);
		frontV[2] = Vertex(edge, edge, -edge,     0, 0, 1,  1, 0);
		frontV[3] = Vertex(-edge, edge, -edge,    0, 0, 1,  0, 0);

		frontBuffer->CreateVertex(4);
		frontBuffer->CreateIndices(INDEX_16BIT, 6);

		frontBuffer->SetIndices(0, 0);
		frontBuffer->SetIndices(1, 1);
		frontBuffer->SetIndices(2, 2);
		frontBuffer->SetIndices(3, 2);
		frontBuffer->SetIndices(4, 3);
		frontBuffer->SetIndices(5, 0);

		for (int32 i = 0;i < 4;i++)
		{
			frontBuffer->SetSourceVertex(i, frontV[i]);
			frontBuffer->SetTransformVertex(i, frontV[i]);
		}

		frontBuffer->SetTexture(front);
		frontBuffer->SetRenderState(RENDER_TRIANGLE, true);
		frontBuffer->SetRenderState(Z_BUFFER, true);

		meshBuffers.push_back(frontBuffer);
	}

	// right
	{
		MeshBuffer* rightBuffer = new MeshBuffer("RIGHT");
		Vertex* rightV = new Vertex[4];
		rightV[0] = Vertex(edge, -edge, -edge,    1, 0, 0, 0, 1);
		rightV[1] = Vertex(edge, -edge, edge,     1, 0, 0, 1, 1);
		rightV[2] = Vertex(edge, edge, edge,      1, 0, 0, 1, 0);
		rightV[3] = Vertex(edge, edge, -edge,     1, 0, 0, 0, 0);

		rightBuffer->CreateVertex(4);
		rightBuffer->CreateIndices(INDEX_16BIT, 6);
		rightBuffer->SetIndices(0, 0);
		rightBuffer->SetIndices(1, 1);
		rightBuffer->SetIndices(2, 2);
		rightBuffer->SetIndices(3, 2);
		rightBuffer->SetIndices(4, 3);
		rightBuffer->SetIndices(5, 0);

		for (int32 i = 0;i < 4;i++)
		{
			rightBuffer->SetSourceVertex(i, rightV[i]);
			rightBuffer->SetTransformVertex(i, rightV[i]);
		}
		rightBuffer->SetTexture(right);
		rightBuffer->SetRenderState(RENDER_TRIANGLE, true);
		rightBuffer->SetRenderState(Z_BUFFER, true);

		meshBuffers.push_back(rightBuffer);
	}

	// back
	{
		MeshBuffer* backBuffer = new MeshBuffer("BACK");
		Vertex* backV = new Vertex[4];
		backV[0] = Vertex(edge, -edge, edge,   0, 0, -1,   0, 1);
		backV[1] = Vertex(-edge, -edge, edge,  0, 0, -1,   1, 1);
		backV[2] = Vertex(-edge, edge, edge,   0, 0, -1,   1, 0);
		backV[3] = Vertex(edge, edge, edge,    0, 0, -1,   0, 0);

		backBuffer->CreateVertex(4);
		backBuffer->CreateIndices(INDEX_16BIT, 6);
		backBuffer->SetIndices(0, 0);
		backBuffer->SetIndices(1, 1);
		backBuffer->SetIndices(2, 2);
		backBuffer->SetIndices(3, 2);
		backBuffer->SetIndices(4, 3);
		backBuffer->SetIndices(5, 0);

		for (int32 i = 0;i < 4;i++)
		{
			backBuffer->SetSourceVertex(i, backV[i]);
			backBuffer->SetTransformVertex(i, backV[i]);
		}

		backBuffer->SetTexture(back);
		backBuffer->SetRenderState(RENDER_TRIANGLE, true);
		backBuffer->SetRenderState(Z_BUFFER, true);

		meshBuffers.push_back(backBuffer);
	}

	// left
	{
		MeshBuffer* leftBuffer = new MeshBuffer();
		Vertex* leftV = new Vertex[4];
		// 0, 1
		// 1, 1
		// 1, 0
		// 0, 0
		leftV[0] = Vertex(-edge, -edge, edge,   -1, 0, 0, 0, 1);
		leftV[1] = Vertex(-edge, -edge, -edge,  -1, 0, 0, 1, 1);
		leftV[2] = Vertex(-edge, edge, -edge,   -1, 0, 0, 1, 0);
		leftV[3] = Vertex(-edge, edge, edge,    -1, 0, 0, 0, 0);

		leftBuffer->CreateVertex(4);
		leftBuffer->CreateIndices(INDEX_16BIT, 6);
		leftBuffer->SetIndices(0, 0);
		leftBuffer->SetIndices(1, 1);
		leftBuffer->SetIndices(2, 2);
		leftBuffer->SetIndices(3, 2);
		leftBuffer->SetIndices(4, 3);
		leftBuffer->SetIndices(5, 0);

		for (int32 i = 0;i < 4;i++)
		{
			leftBuffer->SetSourceVertex(i, leftV[i]);
			leftBuffer->SetTransformVertex(i, leftV[i]);
		}

		leftBuffer->SetTexture(left);
		leftBuffer->SetRenderState(RENDER_TRIANGLE, true);
		leftBuffer->SetRenderState(Z_BUFFER, true);

		meshBuffers.push_back(leftBuffer);
	}

}

void SkyBoxSceneNode::Render(bool updateChild)
{
	SceneObject::Render(updateChild);
}

void SkyBoxSceneNode::Update(bool updateChild)
{
	Camera* camera = gameHost->GetCamera();

	if (camera->IsPerspective()) 
	{
		 float32 viewDistance = (camera->GetNearPlane() + camera->GetFarPlane()) * 0.5F;

		//viewDistance = 35;

		Matrix4x4 translate;
		translate.SetIdentity();
		translate.SetPosition(camera->GetLocalPosition());
		translate.SetScale(viewDistance, viewDistance, viewDistance);

		localToWorldMatrix = translate;
		globaLocalToWorldMatrix = translate;
	}

}

void SkyBoxSceneNode::Destroy()
{
	Debug::LogInfo("移除天空盒子网格!!");

	SafeDelete(worldBounds);
	SafeDelete(sourceBounds);

	if (meshBuffers.size())
	{
		for (uint32 i = 0;i < meshBuffers.size();i++)
		{
			meshBuffers[i]->Destroy();
			delete meshBuffers[i];
			meshBuffers[i] = 0;
		}
		meshBuffers.clear();
	}
	SceneObject::Destroy();
}