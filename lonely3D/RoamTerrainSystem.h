#pragma once
#ifndef __RoamTerrainSystem_H__
#define __RoamTerrainSystem_H__

#include "Terrain.h"

class RoamTerrainSection;
class TriangleNode;
class GameHost;
class RoamTerrainSystem:public Terrain
{
public:
	virtual void Create(
		QuadTree* tree,
		Texture* heightMap,
		AABB* worldSize,
		uint16 shift,
		GameHost* _gameHost);
	RoamTerrainSection* FindSection(int16 sectionX, int16 sectionY);


	virtual void SetTexture(Texture* texture);
	virtual void SetRenderState(RENDER_STATE_TYPE State, uint32 value);
	virtual void SetMaterial(lMaterial* material);
	virtual void SetTextureMulColor(bool _textureMulColor);
	virtual void SetShaderType(SHADER_TYPE type);
	virtual void SetHeightScale(float32 scale);

	virtual void BuildSector();
	virtual void CreateSector();

	void SetErrorScale(float32 s) { errorScale = s; }
	void SetRatioLimit(float32 r) { ratioLimit = r; }

	float32 GetES()const { return errorScale; }
	float32 GetRL()const { return ratioLimit; }

	TriangleNode* CreateNode();
	void Reset();
	void SetForceSplit(bool forceSplit);

	virtual void Destroy();

private:
	RoamTerrainSection* roamSectionArray;
	TriangleNode* nodePool;

	uint32 nodeIndex;

	// 地形lod 的参数
	float32 es;
	float32 rl;

	float32 errorScale;
	float32 ratioLimit;

};

#endif

