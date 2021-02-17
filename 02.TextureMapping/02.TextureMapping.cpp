// 02.TextureMapping.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <GameHost.h>
#include "CubeSceneNode.h"
#include "EmptySceneNode.h"
#include "QuadTreeSceneNode.h"

static int32 renderState[RENDER_STATE_MAX];
static int32 filterMode = 1;

class MyGameHost :public GameHost
{
public:


protected:
    virtual void GameInit();
    virtual void UpdateNode(ISceneNode* node);

    void AddRenderStateNodes(ISceneNode* node);
	void AddRenderTexture(Texture* texture);

private:
    vector<ISceneNode*> renderStateNodes;
	vector<Texture*> renderTexture;

};

void MyGameHost::AddRenderTexture(Texture* texture)
{
	renderTexture.push_back(texture);
}

void MyGameHost::AddRenderStateNodes(ISceneNode* node)
{
    renderStateNodes.push_back(node);
}

void MyGameHost::UpdateNode(ISceneNode* node)
{
	static float32 rotationSpeed = 0.1F;
	static float32 lateralSpeed = 10.0f;
	static float32 zSpeed = 1.0F;

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

	// 渲染状态
	{
		// z-buffer
		if (GetAsyncKeyState('Z') & 0x8000)// zbuffer
		{
			renderState[Z_BUFFER] = !renderState[Z_BUFFER];

			for (uint32 i = 0;i < renderStateNodes.size();i++)
			{
				renderStateNodes[i]->SetRenderState(Z_BUFFER, renderState[Z_BUFFER]);
			}
		}

		// 背面剔除
		if (GetAsyncKeyState('M') & 0x8000)
		{
			// ON_MOVE_BACKFACE
			renderState[ON_MOVE_BACKFACE] = !renderState[ON_MOVE_BACKFACE];

			for (uint32 i = 0;i < renderStateNodes.size();i++)
			{
				renderStateNodes[i]->SetRenderState(ON_MOVE_BACKFACE, renderState[ON_MOVE_BACKFACE]);
			}
		}

		// 线条渲染
		if (GetAsyncKeyState('Q') & 0x8000)
		{
			// RENDER_WIRE_FRAME
			renderState[RENDER_WIRE_FRAME] = !renderState[RENDER_WIRE_FRAME];
			for (uint32 i = 0;i < renderStateNodes.size();i++)
			{
				renderStateNodes[i]->SetRenderState(
					RENDER_WIRE_FRAME, renderState[RENDER_WIRE_FRAME]);
			}
		}
		// 法线
		if (GetAsyncKeyState('N') & 0x8000)
		{
			// RENDER_NORMAL
			renderState[RENDER_NORMAL] = !renderState[RENDER_NORMAL];
			for (uint32 i = 0;i < renderStateNodes.size();i++)
			{
				renderStateNodes[i]->SetRenderState(RENDER_NORMAL, renderState[RENDER_NORMAL]);
			}
		}

		// BOX RENDER_BOX
		if (GetAsyncKeyState('B') & 0x8000)
		{
			// RENDER_BOX
			renderState[RENDER_BOX] = !renderState[RENDER_BOX];
			for (uint32 i = 0;i < renderStateNodes.size();i++)
			{
				renderStateNodes[i]->SetRenderState(RENDER_BOX, renderState[RENDER_BOX]);
			}
		}

		if (GetAsyncKeyState('T') & 0x8000)
		{
			// RENDER_BOX
			renderState[RENDER_TRIANGLE] = !renderState[RENDER_TRIANGLE];
			for (uint32 i = 0;i < renderStateNodes.size();i++)
			{
				renderStateNodes[i]->SetRenderState(RENDER_TRIANGLE,
					renderState[RENDER_TRIANGLE]);
			}
		}

		

		if (GetAsyncKeyState('L') & 0x8000)
		{
			// RENDER_BOX
			filterMode = !filterMode;
			for (uint32 i = 0;i < renderTexture.size();i++)
			{
				renderTexture[i]->SetFilterMode(filterMode);
			}
		}

		sprintf(buffer,
			"Z=ZBuffer[%d],M=Move_Backface[%d],Q=Render_Wire_Frame[%d]",
			renderState[Z_BUFFER], renderState[ON_MOVE_BACKFACE], renderState[RENDER_WIRE_FRAME]);
		DrawTextGDI(300, 500, RGB(0, 0, 255), buffer);

		sprintf(buffer,
			"T=Render_Triangle[%d],N=Render_Normal[%d],B=Render_Box[%d]",
			renderState[RENDER_TRIANGLE], renderState[RENDER_NORMAL], renderState[RENDER_BOX]);
		DrawTextGDI(300, 520, RGB(0, 0, 255), buffer);

		sprintf(buffer,"L=Texture_FilterMode[%d]",filterMode);
		DrawTextGDI(300, 540, RGB(0, 0, 255), buffer);
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
	Debug::LogInfo("02.TextureMapping::GameInit....");
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
	renderState[ON_MOVE_BACKFACE] = true;
	renderState[RENDER_WIRE_FRAME] = false;
	renderState[RENDER_NORMAL] = false;
	renderState[RENDER_BOX] = false;

	const string bgTextureName = "../media/texture/wood01.bmp";
	Texture* texture = GetTextureManager()->LoadResource(bgTextureName);


	ColorF c = WHITE_F;
	c.SetAlpha(0.5F);

	CubeSceneNode* cubeNode = CreateCubeNode(
		Vector4(-10, 10, 35, 1),
		Vector3(1.0F, 1.0F, 1.0F),
		Vector3(0, -31, 0));
	cubeNode->SetRenderState(Z_BUFFER, renderState[Z_BUFFER]);
	// TRANSPARENTT
	cubeNode->SetRenderState(TRANSPARENTT, true);
	cubeNode->SetRenderState(RENDER_TRIANGLE, renderState[RENDER_TRIANGLE]);
	cubeNode->SetRenderState(ON_MOVE_BACKFACE, renderState[ON_MOVE_BACKFACE]);
	cubeNode->SetVertexColor(c);
	cubeNode->SetTexture(0, texture);
	cubeNode->SetShaderType(TEXTURE_GOURAUD_ALPHA);

	const string textName = "../media/texture/panel02.bmp";
	Texture* texture2 = GetTextureManager()->LoadResource(textName);
	texture2->SetFilterMode(filterMode);
	AddRenderTexture(texture2);

	CubeSceneNode* cubeNode2 = CreateCubeNode(
		Vector4(-10, 0, 50, 1),
		Vector3(1, 1, 1),
		Vector3(0, 150, 0));
	cubeNode2->SetRenderState(Z_BUFFER, renderState[Z_BUFFER]);
	cubeNode2->SetRenderState(GOURAUD, true);
	cubeNode2->SetRenderState(RENDER_TRIANGLE, renderState[RENDER_TRIANGLE]);
	cubeNode2->SetRenderState(ON_MOVE_BACKFACE, renderState[ON_MOVE_BACKFACE]);
	cubeNode2->SetVertexColor(WHITE_F);
	cubeNode2->SetTexture(0, texture2);
	cubeNode2->SetShaderType(TEXTURE_GOURAUD);

	CubeSceneNode* cubeNode3 = CreateCubeNode(
		Vector4(20, 10, 35, 1),
		Vector3(1.0F, 1.0F, 1.0F),
		Vector3(0, -31, 0));
	cubeNode3->SetRenderState(Z_BUFFER, renderState[Z_BUFFER]);
	// TRANSPARENTT
	cubeNode3->SetRenderState(TRANSPARENTT, true);
	cubeNode3->SetRenderState(RENDER_TRIANGLE, renderState[RENDER_TRIANGLE]);
	cubeNode3->SetRenderState(ON_MOVE_BACKFACE, renderState[ON_MOVE_BACKFACE]);
	cubeNode3->SetVertexColor(c);
	cubeNode3->SetTexture(0, texture2);
	cubeNode3->SetShaderType(TEXTURE_GOURAUD);

	//cubeNode2->AttachToParent(cubeNode);

	RegisteredFocus(cubeNode);

	AddRenderStateNodes(cubeNode);
	AddRenderStateNodes(cubeNode2);

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
