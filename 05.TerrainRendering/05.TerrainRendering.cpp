// 05.TerrainRendering.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <GameHost.h>
#include "CubeSceneNode.h"
#include "EmptySceneNode.h"
#include "QuadTreeSceneNode.h"
#include "TerrainSection.h"
static int32 renderState[RENDER_STATE_MAX];

class MyGameHost :public GameHost
{
protected:
    virtual void GameInit();
    virtual void UpdateNode(ISceneNode* node);
	virtual void OutputLog(int32 renderCount, int32 coneNodeCount);

    void AddRenderStateNodes(TerrainSection* node);

private:
    vector<TerrainSection*> renderStateNodes;
	int32 _coneNodeCount;
	int32 _renderCount;
};

void MyGameHost::OutputLog(int32 renderCount, int32 coneNodeCount)
{
	_coneNodeCount = coneNodeCount;
	_renderCount = renderCount;
}

void MyGameHost::AddRenderStateNodes(TerrainSection* node)
{
    renderStateNodes.push_back(node);
}

void MyGameHost::UpdateNode(ISceneNode* node)
{
	static float32 rotationSpeed = 0.1F;
	static float32 lateralSpeed = 10.0f;
	static float32 zSpeed = 1.0F;

	// 总物体
	sprintf(buffer, "Total Object Count: %d", totalNodeCount);
	DrawTextGDI(0, 0, RGB(0, 0, 255), buffer);

	// 在四叉树内的物体数量
	sprintf(buffer, "Number of objects in Quadtree: %d", quadNodeCount);
	DrawTextGDI(0, 20, RGB(0, 0, 255), buffer);

	// 在视锥体BOX内的物体数量
	sprintf(buffer, "Number of objects in cone box: %d", _coneNodeCount);
	DrawTextGDI(0, 40, RGB(0, 0, 255), buffer);

	// 经过裁剪后的物体数量
	sprintf(buffer, "Number of objects to render: %d", _renderCount);
	DrawTextGDI(0, 60, RGB(0, 0, 255), buffer);

	// terrainSectionCount
	sprintf(buffer, "Terrain Section Number: %d", terrainSectionCount);
	DrawTextGDI(0, 80, RGB(0, 0, 255), buffer);

	// 三角片的数量
	sprintf(buffer, "Number of triangles: %d", triangleCount);
	DrawTextGDI(0, 100, RGB(0, 0, 255), buffer);

	// 左右旋转
	{
		if (GetAsyncKeyState(VK_RIGHT) & 0x8000)
		{
			node->SetRotateY(1);
		}
		else if (GetAsyncKeyState(VK_LEFT) & 0x8000)
		{
			node->SetRotateY(-1);
		}
	}

	// 上下旋转
	{
		if (GetAsyncKeyState(VK_UP) & 0x8000)
		{
			node->SetRotateX(1);
		}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000)
		{
			node->SetRotateX(-1);
		}
	}

	//上下
	{
		if (GetAsyncKeyState('R') & 0x8000)
		{
			node->Translate(Vector3(0, zSpeed, 0));
		}
		else if (GetAsyncKeyState('F') & 0x8000)
		{
			node->Translate(Vector3(0, -zSpeed, 0));
		}
	}

	// 前后
	{
		if (GetAsyncKeyState('W') & 0x8000)
		{
			node->Translate(Vector3(0, 0, zSpeed));
		}
		else if (GetAsyncKeyState('S') & 0x8000)
		{
			node->Translate(Vector3(0, 0, -zSpeed));
		}
	}

	// 左右
	{
		if (GetAsyncKeyState('A') & 0x8000)
		{
			node->Translate(Vector3(-zSpeed, 0, 0));
		}
		else if (GetAsyncKeyState('D') & 0x8000)
		{
			node->Translate(Vector3(zSpeed, 0, 0));
		}
	}

	// 地形参数
	if (terrainSystem && terrainSystem->IsEnable())
	{
		// 0.008F
		static float32 vRatioLimit = 0.045F;
		static float32 vErrorScale = 40.0F;

		// 阈值增减
		{
			if (GetAsyncKeyState(VK_ADD) & 0x8000)
			{
				vRatioLimit += 0.001F;
			}
			else if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000)
			{
				vRatioLimit -= 0.001F;
			}
		}

		// 误差范围增加
		{
			if (GetAsyncKeyState(VK_MULTIPLY) & 0x8000)  // 乘法键
			{
				vErrorScale += 1.0f;
			}
			else if (GetAsyncKeyState(VK_DIVIDE) & 0x8000)  // 除法键
			{
				vErrorScale -= 1.0f;
			}
		}

		if (vRatioLimit < 0.001f) vRatioLimit = 0.001f;
		if (vErrorScale < 0.001f) vErrorScale = 0.001f;

		terrainSystem->SetErrorScale(vErrorScale);
		terrainSystem->SetRatioLimit(vRatioLimit);

		// 地形LOD参数
		float32 errorScale = terrainSystem->GetES();
		float32 r = terrainSystem->GetRL();

		sprintf(buffer, "Terrian LOD ErrorScale * /: %f", errorScale);
		DrawTextGDI(0, 120, RGB(0, 0, 255), buffer);

		sprintf(buffer, "Terrian LOD RatioLimit + -: %f", r);
		DrawTextGDI(0, 140, RGB(0, 0, 255), buffer);
	}

	{
		// 法线
		if (GetAsyncKeyState('N') & 0x8000)
		{
			// RENDER_NORMAL
			renderState[RENDER_NORMAL] = !renderState[RENDER_NORMAL];
			terrainSystem->SetRenderState(RENDER_NORMAL, renderState[RENDER_NORMAL]);
		}

		// BOX RENDER_BOX
		if (GetAsyncKeyState('B') & 0x8000)
		{
			// RENDER_BOX
			renderState[RENDER_BOX] = !renderState[RENDER_BOX];
			terrainSystem->SetRenderState(RENDER_BOX, renderState[RENDER_BOX]);
		}
		// 线条渲染
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			// RENDER_WIRE_FRAME
			renderState[RENDER_WIRE_FRAME] = !renderState[RENDER_WIRE_FRAME];
			terrainSystem->SetRenderState(RENDER_WIRE_FRAME, renderState[RENDER_WIRE_FRAME]);
		}

		sprintf(buffer,
			"B=Render_Box[%d],N=Render_Normal[%d],Q=Render_Wire_Frame[%d]",
			renderState[RENDER_BOX], renderState[RENDER_NORMAL], renderState[RENDER_WIRE_FRAME]);
		DrawTextGDI(300, 500, RGB(0, 0, 255), buffer);
	}

	if (node->IsCanUpdateMatrix())
	{
		node->Update(true);
	}

	Vector4 pos = node->GetLocalPosition();

	sprintf(buffer, "Object NodeID =%d,Child Size=%d", node->GetNodeID(), node->ChildCount());
	DrawTextGDI(0, 540, RGB(0, 0, 255), buffer);

	sprintf(buffer, "Object Position [%5.2f, %5.2f, %5.2f]", pos.x, pos.y, pos.z);
	DrawTextGDI(0, 520, RGB(0, 0, 255), buffer);

	Vector3 euler = node->GetLocalEulerAngle();
	sprintf(buffer, "Object EulerAngle [%5.2f, %5.2f, %5.2f]", euler.x, euler.y, euler.z);
	DrawTextGDI(0, 500, RGB(0, 0, 255), buffer);
}

void MyGameHost::GameInit()
{
	Debug::LogInfo("01.SoftwareRender::GameInit....");
	/* 坐标系
	   +y
	   ^   +z
	   |  /
	   | /
	   *---->+x
   */

	const string topTextureName = "../media/texture/Sky_Up+Y.png";
	const string bottomTextureName = "../media/texture/Sky_Down-Y.png";

	const string leftTextureName = "../media/texture/Sky_Left+X.png";
	const string rightTextureName = "../media/texture/Sky_Right-X.png";

	const string frontTextureName = "../media/texture/Sky_Back-Z.png";
	const string backTextureName = "../media/texture/Sky_Front+Z.png";

	skeyBox = CreateSkyBoxSceneNode(
		topTextureName,
		bottomTextureName,
		leftTextureName,
		rightTextureName,
		frontTextureName,
		backTextureName);

	for (int32 i = 0;i < 32;i++)
	{
		renderState[i] = 0;
	}

	renderState[Z_BUFFER] = true;
	renderState[RENDER_TRIANGLE] = true;
	renderState[ON_MOVE_BACKFACE] = false;
	renderState[RENDER_WIRE_FRAME] = false;
	renderState[RENDER_NORMAL] = false;
	renderState[RENDER_BOX] = false;

	camera->SetPosition(100, 160, -100);
	camera->SetRotateX(20);
	RegisteredFocus(camera);

	//创建地形，高度图
	const string heightMapName = "../media/texture/heightmap01.bmp";
	Texture* heightMap = textureManager.LoadResource(heightMapName);

	//Debug::LogError("高度图大小="+Convert(heightMap->GetHeight()));

	// SoilCracked
	const string soilCrackedName = "../media/texture/terrain-texture.jpg";
	Texture* soilCracked = GetTextureManager()->LoadResource(soilCrackedName);

	AABB worldExtents(-100, 100, -100, 100, 1.0F, 500.0F);     // z0-z1
	terrainSystem->Create(0, heightMap, &worldExtents, 4, this);
	terrainSystem->SetHeightScale(70.0F);
	terrainSystem->CreateSector();

	// RENDER_WIRE_FRAME RENDER_TRIANGLE
	terrainSystem->SetRenderState(RENDER_WIRE_FRAME, renderState[RENDER_WIRE_FRAME]);
	terrainSystem->SetRenderState(RENDER_TRIANGLE, renderState[RENDER_TRIANGLE]);
	terrainSystem->SetRenderState(Z_BUFFER, renderState[Z_BUFFER]);
	terrainSystem->SetRenderState(RENDER_NORMAL, renderState[RENDER_NORMAL]);
	//地形定点构建有些问题，背面剔除暂时不管用
	terrainSystem->SetRenderState(ON_MOVE_BACKFACE, renderState[ON_MOVE_BACKFACE]);
	terrainSystem->SetTexture(soilCracked);
	terrainSystem->SetShaderType(TERRAIN_SHADER);
	//terrainSystem->SetForceSplit(true);

	vector<ISceneNode*> allNodes = root->GetAllNodes();
	if (allNodes.size()) totalNodeCount = allNodes.size();

	vector<SceneObject*> quadTreeNodes = quadTreeNode->GetQuadTree()->GetAllObject();
	if (quadTreeNodes.size()) quadNodeCount = quadTreeNodes.size();
	terrainSectionCount = terrainSystem->GetSectionCount();

}


int main()
{
	Debug::SetLevel(CL_Debug);

	MyGameHost* gameG = new MyGameHost();

	gameG->createWindow();
	gameG->Run();

	system("pause");
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
