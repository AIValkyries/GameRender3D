#include "TerrainMeshBuffer.h"
#include "Material.h"
#include "Texture.h"
#include "AABB.h"
#include "Vertex.h"
#include "IndexBuffer.h"
#include "Material.h"


lMaterial* TerrainMeshBuffer::GetMaterial()
{
	return material;
}

Texture* TerrainMeshBuffer::GetTexture()
{
	return texture;
}

AABB* TerrainMeshBuffer::GetAABB()
{
	return aabb;
}

void TerrainMeshBuffer::SetAABB(AABB* box)
{
	aabb = box;
}

void TerrainMeshBuffer::RecalculateAABB()
{
	//TODO
}

Vertex* TerrainMeshBuffer::GetTransformVertex(int32 index)
{
	return &transfromBuffer[index];
}

Vertex* TerrainMeshBuffer::GetSourceVertex(int32 index)
{
	return &sourceBuffer[index];
}

IndexBuffer* TerrainMeshBuffer::GetIndexBuffer()
{
	return indexBuffer;
}

void TerrainMeshBuffer::SetTransformVertex(Vertex* vertexs)
{
	transfromBuffer = vertexs;
}

void TerrainMeshBuffer::SetSourceVertex(Vertex* vertexs)
{
	sourceBuffer = vertexs;
}

void TerrainMeshBuffer::SetIndices(IndexBuffer* _indexBuffer)
{
	indexBuffer = _indexBuffer;
}

void TerrainMeshBuffer::SetMaterial(lMaterial* _material)
{
	material = _material;
}


void TerrainMeshBuffer::SetTexture(Texture* _texture)
{
	texture = _texture;
}

void TerrainMeshBuffer::SetIndicesValue(uint32 index, int32 value)
{
	if (indexBuffer)
	{
		indexBuffer->SetValue(index, value);
	}
}

void TerrainMeshBuffer::SetRenderState(RENDER_STATE_TYPE State, uint32 value)
{
	renderState[State] = value;
}

bool TerrainMeshBuffer::CheckRenderState(RENDER_STATE_TYPE State)
{
	return  renderState[State] != 0;
}
