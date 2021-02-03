#pragma once
#ifndef __Terrain_H__
#define __Terrain_H__

#include "DataType.h"
#include "QuadTree.h"
#include "Texture.h"

/// 
/// 
/// <summary>
/// 柏林噪声
/// {
///     一个256大小的向量表 360方向
///     一个256大小的随机索引表
///     两个向量，一个顶点向量，一个点到顶点的向量，
///     计算两个向量的梯度 Dot(v1,v2);
///     6t^5-15t^4+10t^3
/// }
/// 
/// LOD-ROAM
/// {
///     ...
///     Split = ES/D > L
///     1.递归计算误差树
///     2.计算视觉距离
///     3.构建二叉树
///     4.构建三角形索引
/// }
/// 混合纹理
/// {
///     指定高度和斜率的最大和最小范围，
///     斜率用法线的z来判定，z接近1.0F则相当于垂直，如同地形中的平地
///     z接近0.0F相当于陡峭的斜坡
/// 
///     a为随机因子
/// }
/// 此模式下的地形总三角片为1046
/// </summary>

/// <summary>
/// 地形数据,用来生成混合纹理
/// </summary>
struct ElevationData
{
	float32 MinHeightZ;
	float32 MaxHeightZ;
	float32 MinNormalZ;
	float32 MaxNormalZ;
	float32 Strength;
};

class TerrainSection;
class GameHost;
class Terrain
{
public:

	virtual void Create(
		QuadTree* tree,
		Texture* heightMap, 
		AABB* worldSize,
		uint16 shift,
		GameHost* _gameHost);
	virtual void Destroy();

	virtual void SetTexture(Texture* texture);
	virtual void SetRenderState(RENDER_STATE_TYPE State, uint32 value);
	virtual void SetMaterial(lMaterial* material);

	// 构建高度图和法线贴图
	void BuildHeightMapAndNormalMap(Texture* _heightMap);
	virtual void BuildSector();
	// 创建子块
	virtual void CreateSector();

	void GenerateBlendImage(
		Texture* texture,
		ElevationData* data,
		int32 elevationDataCount);

	Vector3& ReadNormal(uint32 x, uint32 y);
	float32& ReadHeight(uint32 x, uint32 y);
	float32& ReadHeight(int32 index);
	float32 ComputeWeight(float32 value, float32 minValue, float32 maxValue);
	uint32 GetMapIndex(uint16 mapX, uint16 mapY);

	uint32 GetHeightMapWidth()const { return  heightMapWidth; }
	uint32 GetHeightMapHeight()const { return  heightMapHeight; }

	TerrainSection* GetAllSection() { return  chunkArray; }
	uint32 GetSectionCount()const;
	uint32 GetSectionXCount()const;
	uint32 GetSectionYCount()const;

	bool IsEnable()const { return isEnable; }

	virtual void SetTextureMulColor(bool _textureMulColor);
	virtual void SetShaderType(SHADER_TYPE type);
	virtual void SetHeightScale(float32 scale);

	uint32 GetTotalDepth()const { return totalDepth; }
	uint32 GetTotalVariances()const { return totalVariances; }

protected:
	GameHost* gameHost;
	uint16 chunkSizeShift;
	AABB* box;
	bool isEnable;

	uint16 heightMapWidth;
	uint16 heightMapHeight;
	Vector2 worldSize;
	Vector3 mapScale;   // 高度图和真实世界的缩放比例

	uint32  sectorVertexs;
	Vector2 sectorCount;
	Vector2 sectorSize;

	TerrainSection* chunkArray;

	float32* heightMap;
	Vector3* normalMap;

	uint32 totalDepth;
	uint32 totalVariances;

};

#endif

