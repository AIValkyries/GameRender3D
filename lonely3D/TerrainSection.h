#pragma once
#ifndef __TerrainSection_H__
#define __TerrainSection_H__

#include "DataType.h"
#include "SceneObject.h"
#include "TerrainMeshBuffer.h"


class Terrain;
class TerrainSection :public SceneObject
{
public:
	virtual void Destroy();
	virtual void Update(bool updateChild);
	virtual void UpdateBox(bool updateChild);
	virtual void Render(bool updateChild);

	virtual void Create(
		Terrain* terrain,
		int32 _sectorX,     int32 _sectorY,
		int32 _heightMapX,  int32 _heightMapY,
		int32 _vertexCount, Vector2 _sectorSize,
		uint32 _chunkSizeShift,
		GameHost* _gameHost);

	void ExtractFourCornersIndex(
		uint32& index0, uint32& index1,
		uint32& index2, uint32& index3,
		uint32& x0, uint32& y0,
		uint32& x1, uint32& y1);

	Vector4& ReadPosition(int32 index);
	void SetHeightScale(float32 scale);

protected:
	Terrain* mainTerrain;
	uint32 chunkSizeShift;
	int32 sectorX;
	int32 sectorY;

	float32 heightScale;
	MeshBuffer* buffer;

	virtual void BuildVertexBuffer();
	virtual void BuildIndexBuffer();

private:

	Vector2 sectorSize;
	int32 vertexCount;
	int32 heightMapX;
	int32 heightMapY;


};

#endif

