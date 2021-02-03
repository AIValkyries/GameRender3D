#pragma once
#ifndef __Terrain_H__
#define __Terrain_H__

#include "DataType.h"
#include "QuadTree.h"
#include "Texture.h"

/// 
/// 
/// <summary>
/// ��������
/// {
///     һ��256��С�������� 360����
///     һ��256��С�����������
///     ����������һ������������һ���㵽�����������
///     ���������������ݶ� Dot(v1,v2);
///     6t^5-15t^4+10t^3
/// }
/// 
/// LOD-ROAM
/// {
///     ...
///     Split = ES/D > L
///     1.�ݹ���������
///     2.�����Ӿ�����
///     3.����������
///     4.��������������
/// }
/// �������
/// {
///     ָ���߶Ⱥ�б�ʵ�������С��Χ��
///     б���÷��ߵ�z���ж���z�ӽ�1.0F���൱�ڴ�ֱ����ͬ�����е�ƽ��
///     z�ӽ�0.0F�൱�ڶ��͵�б��
/// 
///     aΪ�������
/// }
/// ��ģʽ�µĵ���������ƬΪ1046
/// </summary>

/// <summary>
/// ��������,�������ɻ������
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

	// �����߶�ͼ�ͷ�����ͼ
	void BuildHeightMapAndNormalMap(Texture* _heightMap);
	virtual void BuildSector();
	// �����ӿ�
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
	Vector3 mapScale;   // �߶�ͼ����ʵ��������ű���

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

