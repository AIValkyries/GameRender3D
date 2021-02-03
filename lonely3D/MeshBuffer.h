#pragma once
#ifndef __lMeshBuffer_H__
#define __lMeshBuffer_H__

#include "DataType.h"
#include "lMathf.h"
#include <vector>
#include "IndexBuffer.h"
#include "Types.h"
#include "Material.h"
#include "Texture.h"
#include "AABB.h"
#include "Vertex.h"

// 接口
// Transform顶点 buffer
// Source顶点 buffer
// 索引 buffer
// 材质(每个buffer一个材质)
// aabb
static int32 MeshID = 0;
#define MAX_TEXTURE_COUNT 4

class MeshBuffer
{
public:
	MeshBuffer();
	MeshBuffer(string _name);

	// 顶点
	 void CreateVertex(int32 count);
	 void SetTransformVertex(Vertex* vertexs);
	 void SetTransformVertex(int32 index,Vertex& vertex);
	 void SetSourceVertex(Vertex* vertexs);
	 void SetSourceVertex(int32 index, Vertex& vertex);

	 // 将一个面的顶点插入末尾
	 void InsertTransformFace(Vertex* faceVertexs);
	 void ResetVertexCount();
	 Vertex* GetTransformVertex(int32 index);
	 Vertex* GetSourceVertex(int32 index);
	 int32 GetVertexCount();

	// 索引
	 void CreateIndices(INDEX_TYPE type, int32 size);
	 void SetIndices(IndexBuffer* indexBuffer);
	 void SetIndices(uint32 index, uint32 value);
	 void PushIndices(uint32& value);
	 void ClearIndices();

	 IndexBuffer* GetIndexBuffer();

	 lMaterial* GetMaterial();
	 void SetMaterial(lMaterial* _material);
	 Texture* GetTexture();
	 Texture* GetTexture(int32 index);
	 int32 GetTextureCount()const { return textureIndex; }
	 void SetTexture(Texture* _texture);

	 AABB* GetAABB();
	 void SetAABB(AABB* box);
	 void ResetAABB(float32 x, float32 y, float32 z);
	 AABB* RecalculateAABB();

	 void CreateFaceState();
	 void SetFaceState(uint32 faceID, uint32 value);
	 bool CheckFaceState(uint32 faceID, uint32 value);
	 void ClearFaceState(uint32 faceID, uint32 value);
	 void ClearFace(uint32 faceID);

	 void SetRenderState(RENDER_STATE_TYPE State, uint32 value);
	 bool CheckRenderState(RENDER_STATE_TYPE State);

	 uint32* GetRenderState() { return renderState; }
	 string GetName()const { return name; }

	 void Destroy();

protected:

	string name;
	Vertex* sourceBuffer;
	Vertex* transfromBuffer;
	IndexBuffer* indexBuffer;
	lMaterial* material;
	AABB* aabb;
	Texture* texture[MAX_TEXTURE_COUNT];
	uint32 renderState[RENDER_STATE_MAX];
	uint32* faceStates;

	vector<uint32> tempIndexs;

	uint32 sourceCount;
	uint32 vertexCount;
	uint32 maxVertexCount;

	int32 textureIndex = 0;
};

 


#endif

