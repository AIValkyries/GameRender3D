// 04.Lighting.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <GameHost.h>
#include "CubeSceneNode.h"
#include "EmptySceneNode.h"
#include "QuadTreeSceneNode.h"
#include "AnimatedMeshSceneNode.h"

static int32 renderState[RENDER_STATE_MAX];

class MyGameHost :public GameHost
{
protected:
	virtual void GameInit();
	virtual void UpdateNode(ISceneNode* node);

	void AddRenderStateNodes(ISceneNode* node);

private:
	vector<ISceneNode*> renderStateNodes;

};

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

	// 光照
	{

		if (GetAsyncKeyState('I') & 0x8000)  // 全局光照
		{
			renderState[AMBIENT_LIGHT] = !renderState[AMBIENT_LIGHT];
			
			for (uint32 i = 0;i < renderStateNodes.size();i++)
			{
				renderStateNodes[i]->SetRenderState(AMBIENT_LIGHT,
					renderState[AMBIENT_LIGHT]);
			}
		}
		if (GetAsyncKeyState('O') & 0x8000)  // 镜面反射
		{
			renderState[SPECULAR_LIGHT] = !renderState[SPECULAR_LIGHT];
			
			for (uint32 i = 0;i < renderStateNodes.size();i++)
			{
				renderStateNodes[i]->SetRenderState(SPECULAR_LIGHT,
					renderState[SPECULAR_LIGHT]);
			}

		}
		if (GetAsyncKeyState('P') & 0x8000) // 漫发射
		{
			renderState[DIFFUSE_LIGHT] = !renderState[DIFFUSE_LIGHT];
			
			for (uint32 i = 0;i < renderStateNodes.size();i++)
			{
				renderStateNodes[i]->SetRenderState(DIFFUSE_LIGHT,
					renderState[DIFFUSE_LIGHT]);
			}
		}

		sprintf(buffer, "I Ambient=%d,O Specular=%d,P Diffuse=%d", 
			renderState[AMBIENT_LIGHT],
			renderState[SPECULAR_LIGHT],
			renderState[DIFFUSE_LIGHT]);
		DrawTextGDI(300, 500, RGB(0, 0, 255), buffer);

		sprintf(buffer, "Light Numbers=%d", lightSpace->GetLightCount());
		DrawTextGDI(300, 520, RGB(0, 0, 255), buffer);
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
	renderState[DIFFUSE_LIGHT] = true;
	renderState[RENDER_WIRE_FRAME] = false;
	renderState[RENDER_NORMAL] = false;

	const string bgTextureName = "../media/texture/wood01.bmp";
	Texture* texture = GetTextureManager()->LoadResource(bgTextureName);
	texture->SetFilterMode(0);

	Vector4 pos1 = Vector4(-20, 0, 35);
	Vector3 scale1 = Vector3::one();
	Vector3 rotate1 = Vector3(0, -31, 0);

	CubeSceneNode* cubeNode = CreateCubeNode(pos1, scale1, rotate1);
	cubeNode->SetRenderState(Z_BUFFER, renderState[Z_BUFFER]);
	cubeNode->SetRenderState(RENDER_TRIANGLE, renderState[RENDER_TRIANGLE]);
	cubeNode->SetRenderState(ON_MOVE_BACKFACE, renderState[ON_MOVE_BACKFACE]);
	cubeNode->SetShaderType(TEXTURE_GOURAUD);
	cubeNode->SetVertexColor(GRAY_F);
	cubeNode->SetRenderState(DIFFUSE_LIGHT, renderState[DIFFUSE_LIGHT]);
	cubeNode->SetTexture(0, texture);

	RegisteredFocus(cubeNode);

	lMaterial* material = new lMaterial();
	material->AmbientColor = ColorF(0.2F, 0.3F, 0.4F);
	material->DiffuseColor = ColorF(0.56F, 0.25F, 0.39F);
	material->SpecularColor = ColorF(0.5F, 0.75F, 0.65F);
	material->Power = 5;
	material->EmissiveColor = ColorF(0, 0, 0, 0);
	cubeNode->SetMaterial(material);

	Vector4 pos2 = Vector4(30, 0, 40);
	Vector3 scale2 = Vector3::one() * 0.15F;
	Vector3 rotate2 = Vector3(0, 180, 0);

	const string filePath = "../media/sphere.mesh";

	AnimatedMeshSceneNode* animated = CreateAnimaateMeshNode(
		filePath, pos2, scale2, rotate2);
	if (animated)
	{
		// 代码设计没做好，设置每秒多少帧必须在设置速度之前...
		animated->SetAnimateFramesPerSecond(25);
		animated->SetSpeed(0.5F);
		animated->SetLoop(true);
		//animated->PlayAnimation("amuse");

		animated->SetRenderState(DIFFUSE_LIGHT, renderState[DIFFUSE_LIGHT]);
		animated->SetRenderState(RENDER_TRIANGLE, true);
		animated->SetRenderState(Z_BUFFER, true);
		animated->SetRenderState(ON_MOVE_BACKFACE, true);
		animated->SetShaderType(GOURAUD_SHADER);
	}

	RegisteredFocus(animated);

	lMaterial* material1 = new lMaterial();
	material1->AmbientColor = ColorF(0.2F, 0.3F, 0.4F);
	material1->DiffuseColor = ColorF(0.56F, 0.25F, 0.39F);
	material1->SpecularColor = ColorF(0.5F, 0.25F, 0.65F);
	material1->Power = 5;
	material1->EmissiveColor = ColorF(0, 0, 0, 0);
	animated->SetMaterial(material1);

	ColorF ambientColor = GRAY_F;
	ColorF diffuseColor = WHITE_F;
	ColorF specularColor = WHITE_F;
	Vector3 direction(0, 0, -1);  // -1
	Vector3 lightPos = Vector3::zero();
	float32 radius = 50;
	float32 falloff = 1.0F;

	Light* light = lightSpace->CreateDirectionalLight(
		ambientColor, diffuseColor, specularColor, direction);

	Light* pointLight = lightSpace->CreatePointLight(
		ambientColor, diffuseColor, specularColor, lightPos, radius);

	LightSceneNode* directionallightNode = CreateLightNode(
		light, Vector4(-10, 0, 35, 0));
	//lightNode->SetRotateY(135);
	directionallightNode->SetRenderState(RENDER_BOX, true);

	LightSceneNode* pointLightNode = CreateLightNode(
		pointLight, Vector4(-10, 0, 35, 0));
	pointLightNode->SetRenderState(RENDER_BOX, true);

	AddRenderStateNodes(animated);
	AddRenderStateNodes(cubeNode);

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
