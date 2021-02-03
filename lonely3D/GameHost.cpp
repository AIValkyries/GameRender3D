#include "stdafx.h"

#include "GameHost.h"
#include "IndexBuffer.h"
#include "Debug.h"
#include <iostream>
#include <vector>
#include <string>
#include "CubeSceneNode.h"
#include "FlatShader.h"
#include "Gouraud.h"
#include "GouraudAlpha.h"
#include "TextureGouraud.h"
#include "TextureGouraudAlpha.h"
#include "GroundTextureBlend.h"
#include "TerrainShader.h"
#include "AnimatedMeshSceneNode.h"
#include "TerrainSection.h"
#include "SkyBoxSceneNode.h"
#include "EmptySceneNode.h"
#include "QuadTreeSceneNode.h"
#include <atlstr.h>
#include "QuadTree.h"
#include "LightSceneNode.h"
#include <sstream>

using namespace std;


#define SET_AMBIENT_LIGHT  1001
#define SET_DIFFUSE_LIGHT  1002
#define SET_SPECULAR_LIGHT 1003
#define WINDOW_CLASS_NAME TEXT("WinClass1")
static const char* const WINDOW_ENTRY_NAME = "Entry";


// 日了狗...
static Vector4 Cross(Vector4 a, Vector4 b)
{
	Vector3 v0(a.x, a.y, a.z);
	Vector3 v1(b.x, b.y, b.z);

	Vector3 value = Cross(v0, v1);

	return Vector4(value.x, value.y, value.z, 1);
}


LRESULT CALLBACK WindowProc(
	HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	HMENU hSysMenu = 0;
	HMENU hMenu;

	switch (msg)
	{
	case WM_CREATE:
	{


	}break;
	case WM_COMMAND:
	{
		hMenu = GetMenu(hwnd);
		switch (LOWORD(wparam))
		{
		case SET_AMBIENT_LIGHT:
		{
			/*uint32 flag = set_ambient ? 0 : E_Ambient;
			device.GetLightSpace().SetFlags(flag);
			set_ambient = !set_ambient;*/

		}break;
		case SET_DIFFUSE_LIGHT:
		{
		/*	uint32 flag = set_diffuse ? 0 : E_Diffuse;
			device.GetLightSpace().SetFlags(flag);
			set_diffuse = !set_diffuse;*/
		}break;
		case SET_SPECULAR_LIGHT:
		{
	/*		uint32 flag = set_specular ? 0 : E_Specular;
			device.GetLightSpace().SetFlags(flag);
			set_specular = !set_specular;*/
		}break;
		}

	}break;
	case WM_PAINT:
	{
		hdc = BeginPaint(hwnd, &ps);

		EndPaint(hwnd, &ps);
	}break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		DestroyWindow(hwnd);
	}break;
	case WM_KEYDOWN:
	{
		if (wparam == VK_ESCAPE)
		{
			PostQuitMessage(0);
			DestroyWindow(hwnd);
		}
	}break;

	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

void GameHost::createWindow()
{
	HINSTANCE hInstance = GetModuleHandle(NULL);

	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WindowProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wcex.hCursor = LoadCursor(hInstance, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.lpszClassName = WINDOW_CLASS_NAME;
	wcex.lpszMenuName = NULL;
	wcex.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	if (!RegisterClassEx(&wcex))
	{
		::MessageBox(0, TEXT("RegisterClass()-FAILED"), 0, 0);
		return;
	}

	HWND hWnd = 0;
	hWnd = CreateWindowEx(
		NULL,
		WINDOW_CLASS_NAME,
		TEXT("Lonely 3D Render"),
		WS_OVERLAPPEDWINDOW | WS_VISIBLE | WS_SYSMENU,
		0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
		NULL, NULL, hInstance, NULL);
	if (!hWnd)
	{
		MessageBox(0, TEXT("CreateWindows()-FAILED"), 0, 0);
		return;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
	HWnd = hWnd;

	OnCreateWindow();
}

void GameHost::SetWindowCaption(const wchar_t* text)
{
	DWORD_PTR dwResult;
	SendMessageTimeout(
		HWnd, WM_SETTEXT, 0,
		reinterpret_cast<LPARAM>(text),
		SMTO_ABORTIFHUNG,
		2000, &dwResult);
}


void GameHost::DrawTextGDI(
	int32 x, int32 y,
	DWORD color, char* text)
{
	HDC xdc = GetDC(HWnd);
	SetTextColor(xdc, color);
	SetBkMode(xdc, TRANSPARENT);

	CString str = CString(text);
	LPCWSTR wszClassName = new WCHAR[str.GetLength() + 1];
	wcscpy((LPTSTR)wszClassName, T2W((LPTSTR)str.GetBuffer(NULL)));

	TextOut(xdc, x, y, wszClassName, strlen(text));
	ReleaseDC(HWnd, xdc);

	str.ReleaseBuffer();
}

QuadTree* GameHost::GetQuadTree()const
{
	if (quadTreeNode)
		return quadTreeNode->GetQuadTree();
	return 0;
}

void GameHost::Run()
{
	GameInit();

	MSG msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		if (PeekMessage(&msg, HWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			GameMain();
		}
	}

	GameShutdown();
}

void GameHost::OnCreateWindow()
{
	lastFps = -1;
	uint32 mwidth  = uint32(SCREEN_WIDTH / 1.25F);
	uint32 mheight = uint32(SCREEN_HEIGHT / 1.25F);

	skinnedManager.InitResourcePool(this);
	textureManager.InitResourcePool(this);

	backBuffer = new Texture();
	backBuffer->CreateTexture(ARGB_32, mwidth, mheight);
	backBuffer->SetFileName("Frame_Buffer");
	depthBuffer = new ZBuffer(mwidth, mheight);

	terrainSystem = new RoamTerrainSystem();
	lightSpace = new LightSpace();
	camera = new Camera();
	timer  = new Timer();
	root = new EmptySceneNode();
	root->Create(this);

	timer->InitTimer();
	camera->CreateDefaultCamera(this);

	PushNode(camera);

	// camera必须是第一个
	quadTreeNode = new QuadTreeSceneNode();
	quadTreeNode->Create(this);
	PushNode(quadTreeNode);

	//RegisteredFocus(quadTreeNode);
	// 
	allShader[GOURAUD_SHADER] = new Gouraud();
	allShader[GOURAUD_ALPHA] = new GouraudAlpha();
	allShader[TEXTURE_GOURAUD] = new TextureGouraud();
	allShader[TEXTURE_GOURAUD_ALPHA] = new TextureGouraudAlpha();
	allShader[GROUND_TEXTURE_BLEND] = new GroundTextureBlend();
	allShader[FLAT_SHADER] = new FlatShader();
	allShader[TERRAIN_SHADER] = new TerrainShader();
}

void GameHost::RegisteredFocus(ISceneNode* node)
{
	focusNode = node;
}

void GameHost::UpdateNode(ISceneNode* node)
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

	// 光照
	{
		static int32 ambient = 0;
		static int32 diffuse = 0;
		static int32 specular = 0;

		if (GetAsyncKeyState('I') & 0x8000)  // 全局光照
		{
			ambient = !ambient;
			node->SetRenderState(AMBIENT_LIGHT, ambient);
		}
		if (GetAsyncKeyState('O') & 0x8000)  // 镜面反射
		{
			specular = !specular;
			node->SetRenderState(SPECULAR_LIGHT, specular);
		}
		if (GetAsyncKeyState('P') & 0x8000) // 漫发射
		{
			diffuse = !diffuse;
			node->SetRenderState(DIFFUSE_LIGHT, diffuse);
		}

		sprintf(buffer, "I Ambient=%d,O Specular=%d,P Diffuse=%d", ambient, specular, diffuse);
		DrawTextGDI(300, 500, RGB(0, 0, 255), buffer);

		sprintf(buffer, "Light Numbers=%d", lightSpace->GetLightCount());
		DrawTextGDI(300, 520, RGB(0, 0, 255), buffer);

	}

	node->Update(true);

	Vector4 pos = node->GetLocalPosition();

	sprintf(buffer, "Object Position [%5.2f, %5.2f, %5.2f]", pos.x, pos.y, pos.z);
	DrawTextGDI(0, 520, RGB(0, 0, 255), buffer);

	Vector3 euler = node->GetLocalEulerAngle();
	sprintf(buffer, "Object EulerAngle [%5.2f, %5.2f, %5.2f]", euler.x, euler.y, euler.z);
	DrawTextGDI(0, 500, RGB(0, 0, 255), buffer);

}

void GameHost::GameInit()
{
	Debug::LogInfo("GameInit....");

    /* 坐标系 
	       +y
	       ^   +z
	       |  /
           | /
		   *---->+x
	*/
	const string topTextureName    = "../media/texture/Sky_Up+Y.png";
	const string bottomTextureName = "../media/texture/Sky_Down-Y.png";

	const string leftTextureName  = "../media/texture/Sky_Left+X.png";
	const string rightTextureName = "../media/texture/Sky_Right-X.png";

	const string frontTextureName = "../media/texture/Sky_Back-Z.png";
	const string backTextureName  = "../media/texture/Sky_Front+Z.png";

	skeyBox = CreateSkyBoxSceneNode(
		topTextureName,
		bottomTextureName,
		leftTextureName,
		rightTextureName,
		frontTextureName,
		backTextureName);

	const float32 scale = 1;

	// 同一个texture只存在一份数据，懒得改了，刚开始没设计好
	// 比如下面3个或者n个cube共享一个 texture
	const string bgTextureName = "../media/texture/wood01.bmp";
	Texture* texture = GetTextureManager()->LoadResource(bgTextureName);
	texture->SetFilterMode(0);

	// -26
	CubeSceneNode* cubeNode = CreateCubeNode(
		Vector4(10, 0, 35, 1),
		Vector3(1.0F, 1.0F, 1.0F),
		Vector3(0, -31, 0));
	cubeNode->SetRenderState(Z_BUFFER, true);
	// TRANSPARENTT
	//cubeNode->SetRenderState(TRANSPARENTT, true);
	cubeNode->SetRenderState(RENDER_TRIANGLE, true);
	//cubeNode->SetRenderState(RENDER_NORMAL, true);
	cubeNode->SetRenderState(ON_MOVE_BACKFACE, true);
	// GOURAUD  FLAT
	cubeNode->SetRenderState(GOURAUD, true);
	//cubeNode->SetRenderState(AMBIENT_LIGHT, true);
	//cubeNode->SetRenderState(DIFFUSE_LIGHT, true);
	//cubeNode->SetRenderState(SPECULAR_LIGHT, true);
	//cubeNode->SetVisible(false);
	cubeNode->SetTexture(0, texture);

	RegisteredFocus(cubeNode);
	//ColorU32 c = WHITE;

	lMaterial* material = new lMaterial();
	material->AmbientColor = ColorF(0.2F, 0.3F, 0.4F);
	material->DiffuseColor = ColorF(0.56F, 0.25F, 0.39F);
	material->SpecularColor = ColorF(0.5F, 0.75F, 0.65F);
	material->Power = 5;
	material->EmissiveColor = ColorF(0, 0, 0, 0);
	cubeNode->SetMaterial(material);

	{
		// penguin.mesh , dwarf.x
		// const string filePath = "../media/sphere.mesh";

		//AnimatedMeshSceneNode* animated = CreateAnimaateMeshNode(
		//	filePath,
		//	Vector4(10, 0, 40),
		//	Vector3::one() * 0.2F,
		//	Vector3::zero());
		//if (animated)
		//{
		//	// 代码设计没做好，设置每秒多少帧必须在设置速度之前...
		//	animated->SetAnimateFramesPerSecond(25);
		//	animated->SetSpeed(0.5F);
		//	animated->SetLoop(true);
		//	animated->PlayAnimation("amuse");

		//	animated->SetRenderState(GOURAUD, true);
		//	animated->SetRenderState(RENDER_TRIANGLE, true);
		//	animated->SetRenderState(Z_BUFFER, true);
		//	animated->SetRenderState(ON_MOVE_BACKFACE, true);
		//}

		//RegisteredFocus(animated);

		//lMaterial* material = new lMaterial();
		//material->AmbientColor = ColorF(0.2F, 0.3F, 0.4F);
		//material->DiffuseColor = ColorF(0.56F, 0.25F, 0.39F);
		//material->SpecularColor = ColorF(0.5F, 0.25F, 0.65F);
		//material->Power = 5;
		//material->EmissiveColor = ColorF(0, 0, 0, 0);
		//animated->SetMaterial(material);
	}

	//CubeSceneNode* cubeNode2 = CreateCubeNode(
	//	Vector4(20, 0, 50, 1),
	//	Vector3(1, 1, 1),
	//	Vector3(0, 150, 0));
	//cubeNode2->SetRenderState(Z_BUFFER, true);
	//cubeNode2->SetRenderState(FLAT, true);
	//cubeNode2->SetRenderState(RENDER_TRIANGLE, true);
	//cubeNode2->SetRenderState(ON_MOVE_BACKFACE, true);
	//cubeNode2->SetTexture(0, texture);
	//typedef unsigned short USHORT;
	//Debug::LogError(Convert(sizeof(USHORT)));
	//Debug::LogError(Convert(sizeof(uint8)));

	ColorF ambientColor = GRAY_F;
	ColorF diffuseColor = WHITE_F;
	ColorF specularColor = WHITE_F;
	Vector3 direction(0, 0, -1);  // -1
	Vector3 lightPos = Vector3::zero();
	float32 radius = 20;
	float32 falloff = 1.0F;

	Light* light = lightSpace->CreateDirectionalLight(
		ambientColor, diffuseColor, specularColor, direction);

	Light* pointLight = lightSpace->CreatePointLight(
		ambientColor, diffuseColor, specularColor, lightPos, radius);

	//Light* spotLight = lightSpace->CreateStandardSpotLight(
	//	ambientColor, diffuseColor, specularColor, lightPos, direction,
	//	falloff, radius);

	LightSceneNode* directionallightNode = CreateLightNode(
		light, Vector4(-10, 0, 35, 0));
	//lightNode->SetRotateY(135);
	//directionallightNode->SetRenderState(RENDER_BOX, true);

	LightSceneNode* pointLightNode = CreateLightNode(
		pointLight, Vector4(-10, 0, 35, 0));
	pointLightNode->SetRenderState(RENDER_BOX, true);

	//RegisteredFocus(cubeNode);

	//cubeNode2->AttachToParent(cubeNode);
	//cubeNode->AttachToParent(cubeNode2);
	//RegisteredFocus(cubeNode2);

	//CubeSceneNode* cubeNode3 = CreateCubeNode(
	//	Vector4(2000, -20, -20),
	//	Vector3(scale, scale, scale),
	//	Vector3::zero());
	//cubeNode3->SetRenderState(Z_BUFFER, true);
	//cubeNode3->SetRenderState(RENDER_TRIANGLE, true);
	//cubeNode3->SetRenderState(ON_MOVE_BACKFACE, true);
	//cubeNode3->SetTexture(0, texture);
	////RegisteredFocus(cubeNode2);
	//cubeNode->AttachToParent(cubeNode2);
	//cubeNode3->AttachToParent(cubeNode);

	// 地形
	{
		// 创建地形，高度图
		//const string heightMapName = "../media/texture/heightmap01.bmp";
		//Texture* heightMap = textureManager.LoadResource(heightMapName);
		////heightMap->CreateTexture(ARGB_32, 16, 16);
		////heightMap->GeneratePerlinNoise(0.01F, 5, 0.6F);

		////Debug::LogError("高度图大小="+Convert(heightMap->GetHeight()));

		//// SoilCracked
		//const string soilCrackedName = "../media/texture/terrain-texture.jpg";
		//Texture* soilCracked = gameHost->GetTextureManager()->LoadResource(soilCrackedName);
		////Texture* grayRock3 = gameHost->GetTextureManager()->LoadResource(grayRock3Name);
		////Texture* shortGrass = gameHost->GetTextureManager()->LoadResource(shortGrassName);
		////
		////Texture* noiseTexture = gameHost->GetTextureManager()->LoadResource(noiseName);
		////// 高度 0-500
		////ElevationData elevation[3];
		////// shortGrass   
		////elevation[0].MinHeightZ = 0;
		////elevation[0].MaxHeightZ = 120;
		////elevation[0].MinNormalZ = -1.0f;
		////elevation[0].MaxNormalZ = 1.0f;
		////elevation[0].Strength = 1.0F;

		////// 
		////elevation[1].MinHeightZ = 100;
		////elevation[1].MaxHeightZ = 150;
		////elevation[1].MinNormalZ = 0.0f;
		////elevation[1].MaxNormalZ = 0.85f;
		////elevation[1].Strength = 1.0F;

		////// soilCracked  
		////elevation[2].MinHeightZ = 150;
		////elevation[2].MaxHeightZ = 200;
		////elevation[2].MinNormalZ = 0.75f;
		////elevation[2].MaxNormalZ = 1.0f;
		////elevation[2].Strength = 1.0F;

		//AABB worldExtents(-100, 100, -100, 100, 1.0F, 500.0F);     // z0-z1
		//terrainSystem->Create(0, heightMap, &worldExtents, 5);

		////terrainSystem->GenerateBlendImage(noiseTexture, elevation, 3);
		////noiseTexture->GenerateRandomChannelNoise(3, 200, 255);

		//// RENDER_WIRE_FRAME RENDER_TRIANGLE
		//terrainSystem->SetRenderState(RENDER_TRIANGLE, true);
		////terrainSystem->SetRenderState(RENDER_WIRE_FRAME, true);
		//terrainSystem->SetRenderState(Z_BUFFER, true);
		//terrainSystem->SetRenderState(RENDER_NORMAL, true);
		////地形定点构建有些问题，背面剔除暂时不管用
		//terrainSystem->SetRenderState(ON_MOVE_BACKFACE, true);
		//terrainSystem->SetTexture(soilCracked);
		//terrainSystem->SetMaterial(material);

		//terrainSystem->SetTexture(shortGrass);
		//terrainSystem->SetTexture(grayRock3);
		//terrainSystem->SetTexture(noiseTexture);
	}

	vector<ISceneNode*> allNodes = root->GetAllNodes();
	if (allNodes.size()) totalNodeCount = allNodes.size();

	vector<SceneObject*> quadTreeNodes = quadTreeNode->GetQuadTree()->GetAllObject();
	if (quadTreeNodes.size()) quadNodeCount = quadTreeNodes.size();
	terrainSectionCount = terrainSystem->GetSectionCount();

}

void GameHost::OutputLog(int32 renderCount, int32 coneNodeCount)
{
	// 总物体
	sprintf(buffer, "Total Object Count: %d", totalNodeCount);
	DrawTextGDI(0, 0, RGB(0, 0, 255), buffer);

	// 在四叉树内的物体数量
	sprintf(buffer, "Number of objects in Quadtree: %d", quadNodeCount);
	DrawTextGDI(0, 20, RGB(0, 0, 255), buffer);

	// 在视锥体BOX内的物体数量
	sprintf(buffer, "Number of objects in cone box: %d", coneNodeCount);
	DrawTextGDI(0, 40, RGB(0, 0, 255), buffer);

	// 经过裁剪后的物体数量
	sprintf(buffer, "Number of objects to render: %d", renderCount);
	DrawTextGDI(0, 60, RGB(0, 0, 255), buffer);

	// terrainSectionCount
	sprintf(buffer, "Terrain Section Number: %d", terrainSectionCount);
	DrawTextGDI(0, 80, RGB(0, 0, 255), buffer);

	// 三角片的数量
	sprintf(buffer, "Number of triangles: %d", triangleCount);
	DrawTextGDI(0, 100, RGB(0, 0, 255), buffer);
}

void GameHost::GameMain()
{
	BeginScene(0, ColorU32(0, 125, 125, 125), 0);

	static int32 renderCount = 0;
	static int32 coneNodeCount = 0;

	if (terrainSystem) 	terrainSystem->Reset();

	if (focusNode) UpdateNode(focusNode);
	if (root)
	{
		root->PreUpdate();

		// 从小到大
		vector<ISceneNode*> allSceneObjects = root->GetAllNodes();
		QuickSortLess(allSceneObjects, 0, allSceneObjects.size() - 1);

		if (allSceneObjects.size())
		{
			for (uint32 i = 0;i < allSceneObjects.size();i++)
			{
				allSceneObjects[i]->Update(false);
				allSceneObjects[i]->UpdateBox(false);
			}
		}

		//for (int32 i = 0;i < allSceneObjects.size();i++)
		//{
		//	Debug::LogError("-------------------------------------------");
		//	Debug::LogError("NODE_ID="+ Convert(allSceneObjects[i]->GetNodeID()));
		//	Debug::LogError("Value=" + Convert(allSceneObjects[i]->GetQueueSortValue()));
		//}
		//root->Update(true);
		//root->UpdateBox(true);
	}
	
	OutputLog(renderCount, coneNodeCount);

	if (quadTreeNode->GetQuadTree() != 0)
	{
		QuadTree* quadTree = quadTreeNode->GetQuadTree();
		const AABB* cameraBox = camera->GetAABB();
		Frustum* frustum = camera->GetFrustum();

		if (cameraBox == 0 || frustum == 0)
		{
			if (root) root->Render(true);
			if (root) root->RenderBox(true);
		}
		else
		{
			triangleCount = 0;

			if (skeyBox)
			{
				skeyBox->Render(false);
				skeyBox->RenderBox(false);
			}
			if (camera)
			{
				camera->Render(false);
				camera->RenderBox(false);
			}

			// 四叉树裁剪
			vector<ISceneNode*> allSceneObjects =
				quadTree->BuildSearchResults(*cameraBox, frustum, coneNodeCount);
			renderCount = allSceneObjects.size();

			// 从小到大排序
			QuickSortLess(allSceneObjects, 0, allSceneObjects.size() - 1);

			for (uint32 i = 0;i < allSceneObjects.size();i++)
			{
				uint32 nodeID = allSceneObjects[i]->GetNodeID();
				allSceneObjects[i]->Render(false);
				allSceneObjects[i]->RenderBox(false);
			}
		}
	}
	else
	{
		triangleCount = 0;

		if (root) root->Render(true);
		if (root) root->RenderBox(true);
	}


	EndScene();

	float32 fps = timer->GetFPS();

	if (lastFps != fps)
	{
		wstring str = L"[Lonely 3D Software Render] FPS:";
		str += to_wstring(fps);
		SetWindowCaption(str.c_str());
		lastFps = int32(fps);
	}

	if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
	{
		PostMessage(HWnd, WM_QUIT, 0, 0);
		//PostMessage(HWnd, WM_DESTROY, 0, 0);
	}

}

void GameHost::GameShutdown()
{
	Debug::LogInfo("GameShutdown....");

	PostQuitMessage(0);
	DestroyWindow(HWnd);

	skinnedManager.DestroyAll();
	textureManager.DestroyAll();

	root->Destroy();

	if (terrainSystem)
	{
		terrainSystem->Destroy();
		SafeDelete(terrainSystem);
	}

	SafeDelete(lightSpace);
	SafeDelete(depthBuffer);
	SafeDelete(backBuffer);
	SafeDelete(timer);

	for (int32 i = 0;i < SHADER_MAX;i++)
	{
		SafeDelete(allShader[i]);
	}

	camera = 0;
}

void GameHost::DrawTexture(Texture* texture)
{
	static IShader* shader = allShader[0];
	shader->SetRenderTarget(backBuffer);
	shader->SetZBuffr(depthBuffer);
	shader->SetTextureParam(texture, 0);
	shader->DrawTexture();
}

void GameHost::DrawPrimitiveNormal(IShader* shader, Vertex* source)
{
	Vector2 v20(source[0].position.x, source[0].position.y);
	Vector2 v21(source[1].position.x, source[1].position.y);
	Vector2 v22(source[2].position.x, source[2].position.y);

	static ColorU32 red(0, 255, 0, 0);

	Vector2 n20 = v20 + Vector2(source[0].normal.x * 15, source[0].normal.y * 15);
	Vector2 n21 = v21 + Vector2(source[1].normal.x * 15, source[1].normal.y * 15);
	Vector2 n22 = v22 + Vector2(source[2].normal.x * 15, source[2].normal.y * 15);

	shader->DrawLine(v20, n20, red);
	shader->DrawLine(v21, n21, red);
	shader->DrawLine(v22, n22, red);
}

/// <summary>
/// 渲染线框三角形
/// </summary>
/// <param name="source"></param>
void GameHost::DrawPrimitiveLine(IShader* shader,Vertex* source)
{
	Vector2 v20(source[0].position.x, source[0].position.y);
	Vector2 v21(source[1].position.x, source[1].position.y);
	Vector2 v22(source[2].position.x, source[2].position.y);

	ColorF c0(Lerp(source[1].color, source[2].color, 0.5F));
	ColorF c1(Lerp(source[2].color, source[0].color, 0.5F));
	ColorF c2(Lerp(source[1].color, source[0].color, 0.5F));

	shader->DrawLine(v21, v22, c0);
	shader->DrawLine(v22, v20, c1);
	shader->DrawLine(v21, v20, c2);
}

void GameHost::DrawPrimitive(
	IShader* shader,
	MeshBuffer* meshBuffer, 
	Vertex* vertexList)
{
	int32  num_verts_in = 0;
	Vertex tempVertexList[3];

	bool isClip = PrimitiveZClip(vertexList, tempVertexList, num_verts_in);
	if (!isClip) return;

	triangleCount++;

	// 视图to投影矩阵,投影 to 屏幕
	Transformation(vertexList, 3);
	if (num_verts_in == 2)
	{
		Transformation(tempVertexList, 3);
		triangleCount++;
	}

	// 渲染
	if (meshBuffer->CheckRenderState(RENDER_TRIANGLE))
	{
		shader->DrawTriangle(&vertexList[0], &vertexList[1], &vertexList[2]);
		if (num_verts_in == 2)
			shader->DrawTriangle(&tempVertexList[0], &tempVertexList[1], &tempVertexList[2]);
	}
	if (meshBuffer->CheckRenderState(RENDER_WIRE_FRAME))
	{
		DrawPrimitiveLine(shader, vertexList);
		if (num_verts_in == 2) DrawPrimitiveLine(shader, tempVertexList);
	}

	if (meshBuffer->CheckRenderState(RENDER_NORMAL))
	{
		DrawPrimitiveNormal(shader, vertexList);
		if (num_verts_in == 2) DrawPrimitiveNormal(shader, tempVertexList);
	}
}

/// <summary>
/// 1.z裁剪,2.光照，3.视图to投影，4.投影to屏幕,5.渲染
/// </summary>
/// <param name="meshBuffer"></param>
void GameHost::DrawVertexPrimitiveList(
	MeshBuffer* meshBuffer,
	Matrix4x4& localToWorldMatrix,
	bool objectIsTextureMulColor,
	int32 shaderType)
{
	IShader* shader = allShader[shaderType];

	shader->EnableTextureMulColor(objectIsTextureMulColor);
	shader->SetRenderTarget(backBuffer);
	shader->SetZBuffr(depthBuffer);
	shader->SetRenderState(meshBuffer->GetRenderState());

	int32 textureCount = meshBuffer->GetTextureCount();
	for (int32 i = 0;i < textureCount;i++)
	{
		shader->SetTextureParam(meshBuffer->GetTexture(i), i);
	}

	if (meshBuffer->CheckRenderState(RENDER_TEXTURE))
	{
		shader->DrawTexture();
		return;
	}

	// 是否开启背面剔除
	if (meshBuffer->CheckRenderState(ON_MOVE_BACKFACE))
	{
		RemoveBackfaces(meshBuffer, camera, shader);
	}
	else
	{
		IndexBuffer* indexBuffer = meshBuffer->GetIndexBuffer();
		int32 primitiveCount = int32(indexBuffer->GetPrimitiveCount());

		for (int32 i = 0;i < primitiveCount;i++)
		{
			meshBuffer->ClearFaceState(i, MOVE_BACKFACE);
		}
	}

	IndexBuffer* indexBuffer = meshBuffer->GetIndexBuffer();

	if (indexBuffer)
	{
		int32  primitiveCount = int32(indexBuffer->GetPrimitiveCount());
		Vertex  vertexList[3];

		int32 move_backface = 0;

		for (int32 i = 0;i < primitiveCount;i++)
		{
			if (meshBuffer->CheckFaceState(i, MOVE_BACKFACE))
			{
				move_backface++;
				continue;
			}

			int32  index = i * 3;
			uint32 index0 = indexBuffer->GetValue(index);
			uint32 index1 = indexBuffer->GetValue(index + 1);
			uint32 index2 = indexBuffer->GetValue(index + 2);

			Vertex* v0_V = meshBuffer->GetTransformVertex(index0);
			Vertex* v1_V = meshBuffer->GetTransformVertex(index1);
			Vertex* v2_V = meshBuffer->GetTransformVertex(index2);

			v0_V->Index = index0;
			v1_V->Index = index1;
			v2_V->Index = index2;

			vertexList[0] = *v0_V;
			vertexList[1] = *v1_V;
			vertexList[2] = *v2_V;

			DrawPrimitive(shader, meshBuffer, vertexList);
		}
	}

}

void GameHost::Draw3DLine(
	const Vector3& start, 
	const Vector3& end,
	ColorF color,
	MeshBuffer* meshBuffer,
	Matrix4x4& localToWorldMatrix,
	int32 shaderType)
{
	IShader* shader = allShader[shaderType];
	shader->SetRenderTarget(backBuffer);
	shader->SetZBuffr(depthBuffer);
	shader->SetRenderState(meshBuffer->GetRenderState());

	Vertex v0(Vector4(start.x, start.y, start.z, 1), color);
	ModelTOWorldView(&v0, localToWorldMatrix);

	Vertex v1(Vector4(end.x, end.y, end.z, 1), color);
	ModelTOWorldView(&v1, localToWorldMatrix);

	//bool v10Clip = LineZClip(&v1, &v0);

	// to投影，to屏幕
	Transformation(&v0, 1);
	Transformation(&v1, 1);

	shader->DrawLine(&v1, &v0, color);
}

void GameHost::DrawBox(
	MeshBuffer* meshBuffer,
	Matrix4x4& localToWorldMatrix,
	int32 shaderType)
{
	IShader* shader = allShader[shaderType];

	shader->SetRenderTarget(backBuffer);
	shader->SetZBuffr(depthBuffer);
	shader->SetRenderState(meshBuffer->GetRenderState());
	shader->SetTextureParam(meshBuffer->GetTexture(), 0);

	AABB* aabb = meshBuffer->GetAABB();
	if (aabb == 0) return;

	//static ColorU32 color(255, 190, 128, 128);
	static ColorU32 color(255, 255, 0, 0);

	Vector4 outVertices[8];
	aabb->GetVertices(outVertices);
	int32  num_verts_in = 0;

	static Vertex vertexs[8];

	for (int32 i = 0;i < 8;i++)
	{
		Vertex v0(outVertices[i], color);
		vertexs[i] = v0;

		ModelTOWorldView(&vertexs[i], localToWorldMatrix);
	}

	// z裁剪  算了就这样吧..好粗糙
	bool v10Clip = LineZClip(&vertexs[1], &vertexs[0]);
	bool v12Clip = LineZClip(&vertexs[1], &vertexs[2]);
	bool v23Clip = LineZClip(&vertexs[2], &vertexs[3]);
	bool v30Clip = LineZClip(&vertexs[3], &vertexs[0]);
	bool v54Clip = LineZClip(&vertexs[5], &vertexs[4]);
	bool v56Clip = LineZClip(&vertexs[5], &vertexs[6]);
	bool v67Clip = LineZClip(&vertexs[6], &vertexs[7]);
	bool v74Clip = LineZClip(&vertexs[7], &vertexs[4]);
	bool v26Clip = LineZClip(&vertexs[2], &vertexs[6]);
	bool v15Clip = LineZClip(&vertexs[1], &vertexs[5]);
	bool v04Clip = LineZClip(&vertexs[0], &vertexs[4]);
	bool v37Clip = LineZClip(&vertexs[3], &vertexs[7]);

	// to投影，to屏幕
	Transformation(vertexs, 8);

	// (1,0),(1,2),(2,3),(3,0)
	// (5,4),(5,6),(6,7),(7,4)
	// (2,6),(1,5),(0,4),(3,7)
	if (v10Clip) shader->DrawLine(&vertexs[1], &vertexs[0], color);
	if (v12Clip) shader->DrawLine(&vertexs[1], &vertexs[2], color);
	if (v23Clip) shader->DrawLine(&vertexs[2], &vertexs[3], color);
	if (v30Clip) shader->DrawLine(&vertexs[3], &vertexs[0], color);

	if (v54Clip) shader->DrawLine(&vertexs[5], &vertexs[4], color);
	if (v56Clip) shader->DrawLine(&vertexs[5], &vertexs[6], color);
	if (v67Clip) shader->DrawLine(&vertexs[6], &vertexs[7], color);
	if (v74Clip) shader->DrawLine(&vertexs[7], &vertexs[4], color);

	if (v26Clip) shader->DrawLine(&vertexs[2], &vertexs[6], color);
	if (v15Clip) shader->DrawLine(&vertexs[1], &vertexs[5], color);
	if (v04Clip) shader->DrawLine(&vertexs[0], &vertexs[4], color);
	if (v37Clip) shader->DrawLine(&vertexs[3], &vertexs[7], color);

}

// CW = 顺时针,CCW = 逆时针
// CW = 正面，也就是说顺时针的三角面为正面
void GameHost::RemoveBackfaces(
	MeshBuffer* meshBuffer,
	Camera* camera,
	IShader* shader)
{
	IndexBuffer* indexBuffer = meshBuffer->GetIndexBuffer();
	int32 primitiveCount = int32(indexBuffer->GetPrimitiveCount());

	uint32 move_backFace = 0;

	for (int32 i = 0;i < primitiveCount;i++)
	{
		int32 index = i * 3;
		int32 index0 = indexBuffer->GetValue(index);
		int32 index1 = indexBuffer->GetValue(index + 1);
		int32 index2 = indexBuffer->GetValue(index + 2);

		// Face
		Vertex* v0 = meshBuffer->GetTransformVertex(index0);
		Vertex* v1 = meshBuffer->GetTransformVertex(index1);
		Vertex* v2 = meshBuffer->GetTransformVertex(index2);

		// 左手坐标系，顺时针指向你
		Vector4 v0_v1 = v1->position - v0->position;
		Vector4 v0_v2 = v2->position - v0->position;

		Vector4 n; // 面法线
		n = Cross(v0_v1, v0_v2);
		n.Normalize();

		Vector4 viewPos = camera->GetLocalPosition();
		Vector4 viewToV0Edge = v0->position - viewPos;

		float32 dot = Dot(viewToV0Edge, n);

		if (dot < 0.0F)
		{
			move_backFace++;
			meshBuffer->SetFaceState(i, MOVE_BACKFACE);
		}
		else
		{
			meshBuffer->ClearFaceState(i, MOVE_BACKFACE);
		}
	}

}

void GameHost::ModelTOWorldView(Vertex* v,Matrix4x4& localToWorldMat)
{
	Matrix4x4 viewMat = camera->GetViewMatrix4x4();
	v->position = localToWorldMat.MultiplyVector4(v->position);
	v->position = viewMat.MultiplyVector4(v->position);
}

void GameHost::Model_2_World(
	MeshBuffer* meshBuffer, 
	Matrix4x4& localToWorldMat, 
	Matrix4x4& normalTranMat)
{
	int32 vertexCount = int32(meshBuffer->GetVertexCount());

	for (int32 i = 0;i < vertexCount;i++)
	{
		Vertex* transformVertex = meshBuffer->GetTransformVertex(i);
		Vertex* sourceVertex = meshBuffer->GetSourceVertex(i);

		transformVertex->position.Clear();
		transformVertex->normal.Clear();

		Vector4 inV = sourceVertex->position;
		Vector4 inN(sourceVertex->normal.x, sourceVertex->normal.y, sourceVertex->normal.z, 1);

		// 模型坐标 to 世界坐标
		transformVertex->position = localToWorldMat.MultiplyVector4(inV);
		// 法线变换矩阵是顶点变换矩阵的逆的转置  S= (M-1)T
		transformVertex->normal = normalTranMat.MultiplyVector4(inN);
		transformVertex->uv = sourceVertex->uv;
		transformVertex->Index = sourceVertex->Index;
		transformVertex->color = sourceVertex->color;
	}
}

void GameHost::World_2_View(MeshBuffer* meshBuffer)
{
	uint32 vertexCount = meshBuffer->GetVertexCount();
	Matrix4x4 viewMat = camera->GetViewMatrix4x4();

	for (uint32 i = 0;i < vertexCount;i++)
	{
		Vertex* transformVertex = meshBuffer->GetTransformVertex(i);
		transformVertex->position = viewMat.MultiplyVector4(transformVertex->position);
	}
}

// 投影和屏幕变换
void GameHost::Transformation(Vertex* source,int32 count)
{
	// 投影变换
	Matrix4x4 projMat = camera->GetProjMatrix4x4();

	for (int32 i = 0;i < count;i++)
	{
		source[i].position = projMat.MultiplyVector4(source[i].position);

		// 屏幕变换
		float32 w = source[i].position.w;

		float32 x = 0;
		float32 y = 0;
		float32 z = 0;

		if (w != 0)
		{
			x = source[i].position.x / w;
			y = source[i].position.y / w;
			z = source[i].position.z / w;
		}
		else
		{
			x = source[i].position.x;
			y = source[i].position.y;
			z = source[i].position.z;
		}

		// 屏幕映射
		float32 halfWidth  = backBuffer->GetWidth() * 0.5F;
		float32 halfHeight = backBuffer->GetHeight() * 0.5F;

		source[i].position.x = x * halfWidth + (halfWidth);
		source[i].position.y = y * halfHeight + (halfHeight);
	}
}

// 线段z裁剪,发生在视图空间
bool GameHost::LineZClip(Vertex* v0, Vertex* v1)
{
	int32 nearZ = int32(camera->GetNearPlane());
	int32 farZ = int32(camera->GetFarPlane());

	if (v0->position.z > v1->position.z)
	{
		Vertex* temp = v0;
		v0 = v1;
		v1 = temp;
	}

	if (v0->position.z < nearZ && v1->position.z < nearZ)
		return false;
	if (v0->position.z > farZ && v1->position.z > farZ)
		return false;

	if (v0->position.z < nearZ)
	{
		Vector4 v = v1->position - v0->position;
		float32 t = (nearZ - v0->position.z) / (v.z);

		float32 xi = v0->position.x + v.x * t;
		float32 yi = v0->position.y + v.y * t;

		v0->position.x = xi;
		v0->position.y = yi;
		v0->position.z = float32(nearZ);
	}

	return true;
}

bool GameHost::PrimitiveZClip(
	Vertex* vertexList, 
	Vertex* tempVertexList, 
	int32& num_verts_in)
{
#define CLIP_CODE_GZ 0x0001  // z > z_max
#define CLIP_CODE_LZ 0x0002  // z < z_min
#define CLIP_CODE_IZ 0x0004  // z_min < z < z_max

	int32 nearZ = int32(camera->GetNearPlane());
	int32 farZ = int32(camera->GetFarPlane());
	int32 v0, v1, v2;

	int32  vertex_ccodes[3];

	//z clip Test
	{
		vertex_ccodes[0] = ZClipTest(&vertexList[0], num_verts_in);
		vertex_ccodes[1] = ZClipTest(&vertexList[1], num_verts_in);
		vertex_ccodes[2] = ZClipTest(&vertexList[2], num_verts_in);
	}

	// 如果全部远离近裁剪平面，或者远离近裁剪平面
	if ((vertex_ccodes[0] == CLIP_CODE_GZ &&
		vertex_ccodes[1] == CLIP_CODE_GZ &&
		vertex_ccodes[2] == CLIP_CODE_GZ) ||

		(vertex_ccodes[0] == CLIP_CODE_LZ &&
			vertex_ccodes[1] == CLIP_CODE_LZ &&
			vertex_ccodes[2] == CLIP_CODE_LZ))
	{
		return false;
	}

	// 默认初始化
	v0 = 0;v1 = 1;v2 = 2;

	// 如果需要裁剪
	if ((vertex_ccodes[0] | vertex_ccodes[1] | vertex_ccodes[2]) & CLIP_CODE_LZ)
	{
		// 参数化方程求解t
		// t = (nearZ - v0.Z)/(v1.Z-v0.Z)
		// 内侧1个顶点，简单情况
		if (num_verts_in == 1)
		{
			// 顶点索引排序
			{
				if (vertex_ccodes[0] == CLIP_CODE_IZ)
				{
					v0 = 0;v1 = 1;v2 = 2;
				}
				else if (vertex_ccodes[1] == CLIP_CODE_IZ)
				{
					v0 = 1;v1 = 2;v2 = 0;
				}
				else
				{
					v0 = 2;v1 = 0;v2 = 1;
				}
			}

			// 1.对边 v0-v1进行裁剪
			Vector4 v = vertexList[v1].position - vertexList[v0].position;
			float32 t1 = (nearZ - vertexList[v0].position.z) / v.z;

			float32 xi = vertexList[v0].position.x + (v.x * t1);
			float32 yi = vertexList[v0].position.y + (v.y * t1);

			vertexList[v1].position.x = xi;
			vertexList[v1].position.y = yi;
			vertexList[v1].position.z = float32(nearZ);

			// 2.对边v2-v0进行裁剪
			v = vertexList[v2].position - vertexList[v0].position;
			float32	t2 = (nearZ - vertexList[v0].position.z) / v.z;
			xi = vertexList[v0].position.x + (v.x * t2);
			yi = vertexList[v0].position.y + (v.y * t2);

			vertexList[v2].position.x = xi;
			vertexList[v2].position.y = yi;
			vertexList[v2].position.z = float32(nearZ);

			// 3.是否存在纹理
			if ((vertexList[v0].uv.x != 0 || vertexList[v0].uv.y != 0) ||
				(vertexList[v1].uv.x != 0 || vertexList[v1].uv.y != 0) ||
				(vertexList[v2].uv.x != 0 || vertexList[v2].uv.y != 0))
			{
				float32 ui = vertexList[v0].uv.x +
					(vertexList[v1].uv.x - vertexList[v0].uv.x) * t1;
				float32 vi = vertexList[v0].uv.y +
					(vertexList[v1].uv.y - vertexList[v0].uv.y) * t1;

				vertexList[v1].uv.x = ui;
				vertexList[v1].uv.y = vi;

				float32 ui1 = vertexList[v0].uv.x + (vertexList[v2].uv.x - vertexList[v0].uv.x) * t2;
				float32 vi1 = vertexList[v0].uv.y + (vertexList[v2].uv.y - vertexList[v0].uv.y) * t2;

				vertexList[v2].uv.x = ui1;
				vertexList[v2].uv.y = vi1;
			}

		}
		else if (num_verts_in == 2)
		{
			// 顶点索引排序
			{
				if (vertex_ccodes[0] == CLIP_CODE_LZ)  // 外侧
				{
					v0 = 0; v1 = 1; v2 = 2;
				}
				else if (vertex_ccodes[1] == CLIP_CODE_LZ)
				{
					v0 = 1; v1 = 2; v2 = 0;
				}
				else
				{
					v0 = 2; v1 = 0; v2 = 1;
				}
			}

			memcpy(tempVertexList, vertexList, sizeof(Vertex) * 3);

			// v0->v1 edge
			Vector4 v = vertexList[v1].position - vertexList[v0].position;
			// 计算t
			float32 t1 = (nearZ - vertexList[v0].position.z) / v.z;
			// 交点1
			float32 xi0 = (vertexList[v0].position.x + v.x * t1);
			float32 yi0 = (vertexList[v0].position.y + v.y * t1);

			// v0->v2 edge
			v = vertexList[v2].position - vertexList[v0].position;
			float32 t2 = (nearZ - vertexList[v0].position.z) / v.z;
			// 交点2
			float32 xi1 = vertexList[v0].position.x + v.x * t2;
			float32 yi1 = vertexList[v0].position.y + v.y * t2;

			vertexList[v0].position.x = xi0;
			vertexList[v0].position.y = yi0;
			vertexList[v0].position.z = float32(nearZ);

			tempVertexList[v1].position.x = xi0;
			tempVertexList[v1].position.y = yi0;
			tempVertexList[v1].position.z = float32(nearZ);

			tempVertexList[v0].position.x = xi1;
			tempVertexList[v0].position.y = yi1;
			tempVertexList[v0].position.z = float32(nearZ);

			if ((vertexList[v0].uv.x != 0 || vertexList[v0].uv.y != 0) ||
				(vertexList[v1].uv.x != 0 || vertexList[v1].uv.y != 0) ||
				(vertexList[v2].uv.x != 0 || vertexList[v2].uv.y != 0))
			{
				float32 ui0 = vertexList[v0].uv.x + (vertexList[v1].uv.x - vertexList[v0].uv.x) * t1;
				float32 vi0 = vertexList[v0].uv.y + (vertexList[v1].uv.y - vertexList[v0].uv.y) * t1;

				float32 ui1 = vertexList[v0].uv.x + (vertexList[v2].uv.x - vertexList[v0].uv.x) * t2;
				float32 vi1 = vertexList[v0].uv.y + (vertexList[v2].uv.y - vertexList[v0].uv.y) * t2;

				vertexList[v0].uv.x = ui0;
				vertexList[v0].uv.y = vi0;

				tempVertexList[v0].uv.x = ui1;
				tempVertexList[v0].uv.y = vi1;
				tempVertexList[v1].uv.x = ui0;
				tempVertexList[v1].uv.y = vi0;
			}
		}
	}

	return true;
}

int32 GameHost::ZClipTest(const Vertex* v, int32& num_verts_in)
{
	if (v->position.z > camera->GetFarPlane())
	{
		return CLIP_CODE_GZ;
	}
	else if (v->position.z < camera->GetNearPlane())
	{
		return CLIP_CODE_LZ;
	}
	else
	{
		num_verts_in++;
		return CLIP_CODE_IZ;
	}
	return 0;
}

void GameHost::BeginScene(DWORD Flags, ColorU32 color, float32 z)
{
	timer->StartClock();
	timer->Tick();
	backBuffer->Fill(color);
	depthBuffer->Clear();
}

void GameHost::EndScene()
{
	timer->WaitClock(30);  // 一秒33帧
	timer->CalculateFPS();
	Present(backBuffer);
}

bool GameHost::Present(Texture* texture)
{
	HDC dc = GetDC(HWnd);

	if (dc)
	{
		RECT rect;
		GetClientRect(HWnd, &rect);

		const void* memory = (const void*)texture->Lock();
		int32 width = texture->GetWidth();
		int32 height = texture->GetHeight();

		BITMAPV4HEADER bitMap;
		ZeroMemory(&bitMap, sizeof(bitMap));

		bitMap.bV4Size = sizeof(BITMAPINFOHEADER);
		bitMap.bV4Height = height;
		bitMap.bV4Width  = width;
		bitMap.bV4Planes = 1;
		bitMap.bV4BitCount = WORD(texture->GetBitsPerPixel());
		bitMap.bV4V4Compression = BI_BITFIELDS;

		bitMap.bV4AlphaMask = texture->GetAlphaMask();
		bitMap.bV4RedMask = texture->GetRedMask();
		bitMap.bV4GreenMask = texture->GetGreenMask();
		bitMap.bV4BlueMask = texture->GetBlueMask();

		StretchDIBits(
			dc,
			0, 0, rect.right, rect.bottom,
			0, 0, width, height,
			memory, (const BITMAPINFO*)(&bitMap),
			DIB_RGB_COLORS, SRCCOPY);
		
		ReleaseDC(HWnd, dc);
	}

	return true;
}

// 快速排序 ，小于基准的在左边，大于基准的在右边
void GameHost::QuickSortLess(vector<ISceneNode*>& objectArrays, int32 left, int32 right)
{
	if (objectArrays.size() <= 0)
		return;

	if (left >= right)
		return;

	ISceneNode* temp = 0;

	// 基准
	ISceneNode* pivot = objectArrays[left];
	int32 i, j;
	i = left;
	j = right;

	while (i < j)
	{
		while (objectArrays[j]->GetQueueSortValue() >= pivot->GetQueueSortValue() && i < j)
			j--;

		while (objectArrays[i]->GetQueueSortValue() <= pivot->GetQueueSortValue() && i < j)
			i++;

		if (i < j)
		{
			temp = objectArrays[i];
			objectArrays[i] = objectArrays[j];
			objectArrays[j] = temp;
		}
	}

	objectArrays[left] = objectArrays[i];
	objectArrays[i] = pivot;

	QuickSortLess(objectArrays, left, i - 1);
	QuickSortLess(objectArrays, i + 1, right);
}

void GameHost::QuickSortGreater(vector<ISceneNode*>& objectArrays, int32 left, int32 right)
{
	if (objectArrays.size() <= 0)
		return;

	if (left >= right)
		return;

	ISceneNode* temp = 0;

	// 基准
	ISceneNode* pivot = objectArrays[left];
	int32 i, j;
	i = left;
	j = right;

	while (i < j)
	{
		while (objectArrays[j]->GetQueueSortValue() <= pivot->GetQueueSortValue() && i < j)
			j--;

		while (objectArrays[i]->GetQueueSortValue() >= pivot->GetQueueSortValue() && i < j)
			i++;

		if (i < j)
		{
			temp = objectArrays[i];
			objectArrays[i] = objectArrays[j];
			objectArrays[j] = temp;
		}
	}

	objectArrays[left] = objectArrays[i];
	objectArrays[i] = pivot;

	QuickSortGreater(objectArrays, left, i - 1);
	QuickSortGreater(objectArrays, i + 1, right);
}


void GameHost::PushNode(ISceneNode* node)
{
	if (root) node->AttachToParent(root);
}

CubeSceneNode* GameHost::CreateCubeNode(
	const Vector4& localPos, 
	const Vector3& localScale, 
	const Vector3& localEuler)
{
	CubeSceneNode* cube = new CubeSceneNode();
	cube->Create(this);
	cube->SetPosition(localPos);
	cube->SetScale(localScale);
	cube->SetRotate(localEuler);
	cube->UpdateMatrix();

	PushNode(cube);
	cube->AttachToQuadTree(quadTreeNode->GetQuadTree());

	return cube;
}

CubeSceneNode* GameHost::CreateCubeNode(
	const Vector4& localPos,
	const Vector3& localEuler)
{
	CubeSceneNode* cube = new CubeSceneNode();
	cube->Create(this);
	cube->SetPosition(localPos);
	cube->SetRotate(localEuler);
	cube->UpdateMatrix();

	PushNode(cube);
	cube->AttachToQuadTree(quadTreeNode->GetQuadTree());

	return cube;
}

CubeSceneNode* GameHost::CreateCubeNode(const Vector4& localPos)
{
	CubeSceneNode* cube = new CubeSceneNode();
	cube->Create(this);
	cube->SetPosition(localPos);
	cube->UpdateMatrix();

	PushNode(cube);
	cube->AttachToQuadTree(quadTreeNode->GetQuadTree());

	return cube;
}

SkyBoxSceneNode* GameHost::CreateSkyBoxSceneNode(
	const string& topName,
	const string& bottomName,
	const string& leftName,
	const string& rightName,
	const string& frontName,
	const string& backName)
{
	Texture* topT = textureManager.LoadResource(topName);
	Texture* bottomT = textureManager.LoadResource(bottomName);
	Texture* leftT = textureManager.LoadResource(leftName);
	Texture* rightT = textureManager.LoadResource(rightName);
	Texture* frontT = textureManager.LoadResource(frontName);
	Texture* backT = textureManager.LoadResource(backName);

	SkyBoxSceneNode* skeyBox = new SkyBoxSceneNode();
	skeyBox->Create(this);
	skeyBox->CreateSkyBox(topT, bottomT, leftT, rightT, frontT, backT);
	skeyBox->UpdateMatrix();
	skeyBox->SetShaderType(TERRAIN_SHADER);

	PushNode(skeyBox);

	return skeyBox;
}

SkyBoxSceneNode* GameHost::CreateSkyBoxSceneNode(
	Texture* top,
	Texture* bottom,
	Texture* left,
	Texture* right,
	Texture* front,
	Texture* back)
{
	SkyBoxSceneNode* skeyBox = new SkyBoxSceneNode();
	skeyBox->Create(this);
	skeyBox->CreateSkyBox(top, bottom, left, right, front, back);
	skeyBox->UpdateMatrix();
	skeyBox->SetShaderType(TERRAIN_SHADER);

	PushNode(skeyBox);

	return skeyBox;
}

AnimatedMeshSceneNode* GameHost::CreateAnimaateMeshNode(
	const string& meshPath,
	const Vector4& localPos,
	const Vector3& localScale,
	const Vector3& localEuler)
{
	SkinnedSkeleton* skinned = skinnedManager.LoadResource(meshPath);
	if (skinned)
	{
		AnimatedMeshSceneNode* animated = new AnimatedMeshSceneNode(skinned);
		animated->Create(this);
		animated->SetPosition(localPos);
		animated->SetScale(localScale);
		animated->SetRotate(localEuler);
		animated->UpdateMatrix();

		PushNode(animated);
		
		animated->AttachToQuadTree(quadTreeNode->GetQuadTree());

		return animated;
	}

	return 0;
}

LightSceneNode* GameHost::CreateLightNode(Light* light)
{
	LightSceneNode* lightNode = new LightSceneNode();
	lightNode->Create(this);
	lightNode->SetLight(light);
	lightNode->AttachToQuadTree(quadTreeNode->GetQuadTree());
	lightNode->UpdateMatrix();

	PushNode(lightNode);
	lightSpace->PushBack(lightNode);

	return lightNode;
}

LightSceneNode* GameHost::CreateLightNode(Light* light, const Vector4& localPos)
{
	LightSceneNode* lightNode = new LightSceneNode();
	lightNode->Create(this);
	lightNode->SetLight(light);
	lightNode->AttachToQuadTree(quadTreeNode->GetQuadTree());
	lightNode->SetPosition(localPos);
	lightNode->UpdateMatrix();

	PushNode(lightNode);
	lightSpace->PushBack(lightNode);

	return lightNode;
}