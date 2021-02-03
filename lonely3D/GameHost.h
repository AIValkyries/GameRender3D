#pragma once
#ifndef __GameHost_H__
#define __GameHost_H__

#include "Color.h"
#include "Geometry.h"
#include "lMathf.h"
#include "ZBuffer.h"
#include "Light.h"
#include "Camera.h"
#include "Timer.h"
#include "MeshBuffer.h"
#include "Singleton.h"
#include "ResourcePool.h"
#include "Debug.h"
#include "IShader.h"
#include "RoamTerrainSystem.h"


typedef ResourcePool<Texture>           TextureManager;
typedef ResourcePool<SkinnedSkeleton>   SkinnedManager;

class AnimatedMeshSceneNode;
class CubeSceneNode;
class EmptySceneNode;
class SkyBoxSceneNode;
class QuadTreeSceneNode;
class LightSceneNode;
class QuadTree;
class Light;

class GameHost
{
public:

	void createWindow();
	void OnCreateWindow();
	void SetWindowCaption(const wchar_t* text);
	void Run();

	bool Present(Texture* texture);

	void DrawVertexPrimitiveList(
		MeshBuffer* meshBuffer, 
		Matrix4x4& localToWorldMatrix,
		bool objectIsEnableLight,
		int32 shaderType);
	void DrawPrimitiveNormal(IShader* shader, Vertex* src);
	void DrawPrimitiveLine(IShader* shader, Vertex* src);
	void DrawPrimitive(
		IShader* shader,
		MeshBuffer* meshBuffer, 
		Vertex* vertexList);

	void DrawBox(
		MeshBuffer* meshBuffer,
		Matrix4x4& localToWorldMatrix,
		int32 shaderType);
	void Draw3DLine(
		const Vector3& start,
		const Vector3& end,
		ColorF color,
		MeshBuffer* meshBuffer,
		Matrix4x4& localToWorldMatrix,
		int32 shaderType);

	void DrawTexture(Texture* texture);
	void DrawTextGDI(
		int32 x, int32 y,
		DWORD color, char* text);

	bool LineZClip(Vertex* v0, Vertex* v1);
	bool PrimitiveZClip(Vertex* vertexList, Vertex* tempList, int32& num_verts_in);
	int32 ZClipTest(const Vertex* v, int32& num_verts_in);
	void RemoveBackfaces(
		MeshBuffer* meshBuffer,
		Camera* camera,
		IShader* shader);
	void Transformation(Vertex* v,int32 count);

	void ModelTOWorldView(
		Vertex* v,
		Matrix4x4& localToWorldMat);

	void Model_2_World(
		MeshBuffer* meshBuffer, 
		Matrix4x4& localToWorldMat,
		Matrix4x4& normalTranMat);
	void World_2_View(MeshBuffer* meshBuffer);

	TextureManager* GetTextureManager() { return &textureManager; }
	SkinnedManager* GetSkinnedManager() { return &skinnedManager; }
	Timer* GetTimer()const { return  timer; }
	Camera* GetCamera()const { return camera; }
	LightSpace* GetLightSpace()const { return lightSpace; }
	QuadTree* GetQuadTree()const;

	void PushNode(ISceneNode* node);

	SkyBoxSceneNode* CreateSkyBoxSceneNode(
		const string& topName,
		const string& bottomName,
		const string& leftName, 
		const string& rightName, 
		const string& frontName, 
		const string& backName);

	SkyBoxSceneNode* CreateSkyBoxSceneNode(
		Texture* top,
		Texture* bottom,
		Texture* left, 
		Texture* right, 
		Texture* front,
		Texture* back);
	AnimatedMeshSceneNode* CreateAnimaateMeshNode(
		const string& meshPath,
		const Vector4& localPos,
		const Vector3& scale, 
		const Vector3& eulerAngle);
	CubeSceneNode* CreateCubeNode(const Vector4& localPos, const Vector3& localScale, const Vector3& localEuler);
	CubeSceneNode* CreateCubeNode(const Vector4& localPos, const Vector3& localEuler);
	CubeSceneNode* CreateCubeNode(const Vector4& localPos);
	LightSceneNode* CreateLightNode(Light* light);
	LightSceneNode* CreateLightNode(Light* light,const Vector4& localPos);

	void GameShutdown();

protected:
	HWND HWnd;
	int32 lastFps;

	// Terrain  RoamTerrainSystem
	RoamTerrainSystem* terrainSystem;
	LightSpace* lightSpace;
	ZBuffer* depthBuffer;
	Texture* backBuffer;
	Camera*  camera;
	Timer*   timer;

	int32 totalNodeCount;
	int32 quadNodeCount;
	int32 terrainSectionCount;
	int32 triangleCount;           // 三角片的数量
	 
	SkyBoxSceneNode* skeyBox;
	QuadTreeSceneNode* quadTreeNode;
	EmptySceneNode* root;
	ISceneNode* focusNode;

	//资源
	SkinnedManager skinnedManager;
	TextureManager textureManager;

	IShader* allShader[SHADER_MAX];
	char buffer[255];                          // 打印文本的char集合

	void BeginScene(DWORD Flags, ColorU32 color, float32 z);
	void EndScene();

	virtual void UpdateNode(ISceneNode* node);
	void RegisteredFocus(ISceneNode* node);

	virtual void GameInit();
	void GameMain();

	virtual void OutputLog(int32 renderCount,int32 coneNodeCount);

	void QuickSortLess(vector<ISceneNode*>& objectArrays, int32 left, int32 right);
	void QuickSortGreater(vector<ISceneNode*>& objectArrays, int32 left, int32 right);

};

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);



#endif

