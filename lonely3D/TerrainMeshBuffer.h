#pragma once
#ifndef __TerrainMeshBuffer_H__
#define __TerrainMeshBuffer_H__

#include "MeshBuffer.h"

class IndexBuffer;
class TerrainMeshBuffer :public MeshBuffer
{
public:
	TerrainMeshBuffer();

	virtual lMaterial* GetMaterial();
	virtual Texture* GetTexture();

	virtual AABB* GetAABB();
	virtual void SetAABB(AABB* box);
	virtual void RecalculateAABB();

	virtual Vertex* GetTransformVertex(int32 index);
	virtual Vertex* GetSourceVertex(int32 index);

	virtual IndexBuffer* GetIndexBuffer();
	virtual void SetRenderState(RENDER_STATE_TYPE State, uint32 value);
	virtual bool CheckRenderState(RENDER_STATE_TYPE State);

	void SetTransformVertex(Vertex* vertexs);
	void SetSourceVertex(Vertex* vertexs);
	void SetIndices(IndexBuffer* indexBuffer);
	void SetMaterial(lMaterial* _material);
	void SetTexture(Texture* _texture);
	void SetIndicesValue(uint32 index,int32 value);

private:
	Vertex* sourceBuffer;
	Vertex* transfromBuffer;
	IndexBuffer* indexBuffer;
	lMaterial* material;
	AABB* aabb;
	Texture* texture;
	uint32 renderState[RENDER_STATE_MAX];
};

inline TerrainMeshBuffer::TerrainMeshBuffer()
{
	memset(renderState, 0, sizeof(renderState));
}




#endif

