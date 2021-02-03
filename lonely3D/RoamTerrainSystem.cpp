#include "RoamTerrainSystem.h"
#include "RoamTerrainSection.h"
#include "GameHost.h"


void RoamTerrainSystem::Create(
	QuadTree* tree,
	Texture* _heightMap,
	AABB* _worldSize,
	uint16 shift,
	GameHost* _gameHost)
{
	gameHost = _gameHost;
	nodePool = new TriangleNode[RoamTerrainSection::TOTAL_TRITREENODES];
	memset(nodePool, 0, sizeof(TriangleNode) * RoamTerrainSection::TOTAL_TRITREENODES);

	Terrain::Create(tree, _heightMap, _worldSize, shift, gameHost);
}

void RoamTerrainSystem::BuildSector()
{
	int32 length = int32(sectorCount.x * sectorCount.y);

	roamSectionArray = new RoamTerrainSection[length];
	//CreateSector();
}

void RoamTerrainSystem::CreateSector()
{
	for (int32 y = 0;y < sectorCount.y;y++)
	{
		for (int32 x = 0;x < sectorCount.x;x++)
		{
			uint32 heightMapX = x << chunkSizeShift;
			uint32 heightMapY = y << chunkSizeShift;

			int32 index = y * (int32)sectorCount.y + x;

			roamSectionArray[index].Create(
				this,
				x, y,
				heightMapX, heightMapY,
				sectorVertexs, sectorSize,
				chunkSizeShift,
				gameHost);

			gameHost->PushNode(&roamSectionArray[index]);
		}
	}
}

RoamTerrainSection* RoamTerrainSystem::FindSection(
	int16 sectionX, int16 sectionY)
{
	if (sectionX >= 0 && sectionX < sectorCount.x &&
		sectionY >= 0 && sectionY < sectorCount.y)
	{
		return &roamSectionArray[sectionY * (int32)sectorCount.x + sectionX];
	}
	return 0;
}

void RoamTerrainSystem::SetTextureMulColor(bool _textureMulColor)
{
	int32 length = int32(sectorCount.x * sectorCount.y);

	for (int32 i = 0;i < length;i++)
	{
		roamSectionArray[i].SetTextureMulColor(_textureMulColor);
	}
}

void RoamTerrainSystem::SetTexture(Texture* texture)
{
	int32 length = int32(sectorCount.x * sectorCount.y);

	for (int32 i = 0;i < length;i++)
	{
		vector<MeshBuffer*> meshBuffers = roamSectionArray[i].GetMeshBuffers();

		for (uint32 j = 0;j < meshBuffers.size();j++)
		{
			meshBuffers[j]->SetTexture(texture);
		}
	}
}

void RoamTerrainSystem::SetRenderState(RENDER_STATE_TYPE State, uint32 value)
{
	int32 length = int32(sectorCount.x * sectorCount.y);

	for (int32 i = 0;i < length;i++)
	{
		vector<MeshBuffer*> meshBuffers = roamSectionArray[i].GetMeshBuffers();

		for (uint32 j = 0;j < meshBuffers.size();j++)
		{
			meshBuffers[j]->SetRenderState(State, value);
		}
	}
}


void RoamTerrainSystem::SetMaterial(lMaterial* material)
{
	int32 length = int32(sectorCount.x * sectorCount.y);

	for (int32 i = 0;i < length;i++)
	{
		vector<MeshBuffer*> meshBuffers = roamSectionArray[i].GetMeshBuffers();

		for (uint32 j = 0;j < meshBuffers.size();j++)
		{
			meshBuffers[j]->SetMaterial(material);
		}
	}
}

TriangleNode* RoamTerrainSystem::CreateNode()
{
	if (nodeIndex < RoamTerrainSection::TOTAL_TRITREENODES)
		return &nodePool[nodeIndex++];
	else
	{
		Debug::LogError("wwwwwwwwwwwwwwwwwwwwwwwww");
	}
	return 0;
}

void RoamTerrainSystem::Reset()
{
	if (isEnable)
	{
		int32 length = int32(sectorCount.x * sectorCount.y);

		for (int32 i = 0;i < length;i++)
		{
			roamSectionArray[i].Reset();
		}

		nodeIndex = 0;
		memset(nodePool, 0, sizeof(TriangleNode) * RoamTerrainSection::TOTAL_TRITREENODES);
	}
}

void RoamTerrainSystem::SetShaderType(SHADER_TYPE type)
{
	int32 length = int32(sectorCount.x * sectorCount.y);
	for (int32 i = 0;i < length;i++)
	{
		roamSectionArray[i].SetShaderType(type);
	}
}

void RoamTerrainSystem::SetForceSplit(bool forceSplit)
{
	int32 length = int32(sectorCount.x * sectorCount.y);
	for (int32 i = 0;i < length;i++)
	{
		roamSectionArray[i].SetForceSplit(forceSplit);
	}
}

void RoamTerrainSystem::SetHeightScale(float32 scale)
{
	int32 length = int32(sectorCount.x * sectorCount.y);
	for (int32 i = 0;i < length;i++)
	{
		roamSectionArray[i].SetHeightScale(scale);
	}
}

void RoamTerrainSystem::Destroy()
{
	Terrain::Destroy();
	SafeDeleteArray(nodePool);
}