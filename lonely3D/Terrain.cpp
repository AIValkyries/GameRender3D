#include "Terrain.h"
#include "TerrainSection.h"
#include "GameHost.h"

void Terrain::Create(
	QuadTree* tree,
	Texture* _heightMap,
	AABB* _worldSize, 
	uint16 shift,
	GameHost* _gameHost)
{
	totalDepth = 2 * shift;
	totalVariances = 1 << totalDepth;

	gameHost = _gameHost;
	isEnable = true;
	chunkSizeShift = shift;
	box = _worldSize;

	heightMapWidth = _heightMap->GetWidth();
	heightMapHeight = _heightMap->GetHeight();

	Vector3 worldSizeV2 = _worldSize->GetSize();

	worldSize.x = worldSizeV2.x;
	worldSize.y = worldSizeV2.y;

	mapScale.x = worldSize.x / heightMapWidth;
	mapScale.y = worldSize.y / heightMapHeight;
	mapScale.z = worldSizeV2.z / 255.0F;

	uint16 chunkMapSize = 1 << shift;   // 一个块的大小
	sectorVertexs = chunkMapSize + 1;

	sectorCount.x = float32(heightMapWidth / chunkMapSize);
	sectorCount.y = float32(heightMapHeight / chunkMapSize);

	sectorSize.x = worldSize.x / sectorCount.x;
	sectorSize.y = worldSize.y / sectorCount.y;

	Debug::LogWarning("--------------------------地形信息-Start--------------------------");
	
	BuildHeightMapAndNormalMap(_heightMap);

	Debug::LogInfo("高度图大小 [WIDTH=" +
		Convert(heightMapWidth) + ",HEIGHT=" + Convert(heightMapHeight) + "]");
	
	Debug::LogInfo("世界大小 [X0=" + 
		Convert(worldSize.x) + ",Y0=" + Convert(worldSize.y) +
		",Z0=" + Convert(255.0F) + "]");

	Debug::LogInfo("世界大小与高度图的缩放比例=", mapScale);
	Debug::LogInfo("一个地形块的大小=" + Convert(chunkMapSize));
	Debug::LogInfo("地形块数量 [X=" +
		Convert(sectorCount.x) +
		",Y=" + Convert(sectorCount.y)
		+ "],总数量=" + Convert(sectorCount.x * sectorCount.y));

	Debug::LogInfo("每块顶点数量=" + Convert(sectorVertexs) +
		",X=" + Convert(sectorVertexs * sectorCount.x) + ",Y=" + Convert(sectorVertexs * sectorCount.y)
		+ ",总顶点数量=" + Convert(sectorVertexs * sectorVertexs * sectorCount.x * sectorCount.y));

	Debug::LogInfo("每个地形块的大小 [X=" + Convert(sectorSize.x) + ",Y=" + Convert(sectorSize.y) + "]");
	Debug::LogWarning("--------------------------地形信息-END--------------------------");

	BuildSector();
}

void Terrain::BuildHeightMapAndNormalMap(Texture* _heightMap)
{
	if (!_heightMap)
		return;

	int32 _width  = _heightMap->GetWidth();
	int32 _height = _heightMap->GetHeight();

	int32 length = _width * _height;

	heightMap = new float32[length];
	normalMap = new Vector3[length];

	float32 min = 999999;
	float32 max = 0;

	ColorU32 maxColor(0);
	ColorU32 minColor(255, 255, 255, 255);

	float32 minLightness = 9999999;
	float32 maxLightness = 0;

	// 高度图
	for (int32 y = 0;y < _height;y++)
	{
		for (int32 x = 0;x < _width;x++)
		{
			// 从高度图中读取数据
			int32   index = y * _width + x;

			ColorU32 color = _heightMap->GetColorU32(x, y);

			float32 iHeight = color.GetLightness();
			float32 realHeight = iHeight / 255.0F;

			if (iHeight < minLightness)
			{
				minLightness = iHeight;
			}
			if (iHeight > maxLightness)
			{
				maxLightness = iHeight;
			}

			if (color < minColor)
			{
				minColor = color;
			}
			if (color > maxColor)
			{
				maxColor = color;
			}

			if (realHeight < min)
			{
				min = realHeight;
			}

			if (realHeight > max)
			{
				max = realHeight;
			}

			// 真实高度数据
			heightMap[index] = realHeight;
		}
	}

	Debug::LogInfo("高度图中--最小Color=", minColor);
	Debug::LogInfo("高度图中--最大Color=", maxColor);

	Debug::LogInfo("高度图中--MinColor_Lightness="+Convert(minColor.GetLightness()));
	Debug::LogInfo("高度图中--MaxColor_LightnessColor="+Convert(maxColor.GetLightness()));

	Debug::LogInfo("高度图中--最小Lightness=" + Convert(minLightness));
	Debug::LogInfo("高度图中--最大Lightness=" + Convert(maxLightness));

	Debug::LogInfo("高度图中--最小高度=" + Convert(min));
	Debug::LogInfo("高度图中--最大高度=" + Convert(max));

	_heightMap->Unlock();

	// 法线图
	Texture* _normalMap = new Texture();
	_normalMap->CreateTexture(ARGB_32, _width, _height);
	_normalMap->GenerateNormalMap(_heightMap, 0.001F);

	uint8* datas = (uint8*)_normalMap->Lock();
	uint32 pitch = _normalMap->GetPitch();

	min = 999999;
	max = 0;

	for (int32 y = 0;y < _height;y++)
	{
		for (int32 x = 0;x < _width;x++)
		{
			int32 index = y * pitch + x * _normalMap->GetBytesPerPixel();
			Vector3 normal;

			normal.z = datas[index + 0] - 127.5F;
			normal.x = datas[index + 1] - 127.5F;
			normal.y = datas[index + 2] - 127.5F;

			normal.Normalize();

			if (normal.z < min)
			{
				min = normal.z;
			}

			if (normal.z > max)
			{
				max = normal.z;
			}

			int32 nIndex = y * _width + x;
			normalMap[nIndex] = normal;
		}
	}

	Debug::LogInfo("法线Z--最小=" + Convert(min));
	Debug::LogInfo("法线Z--最大=" + Convert(max));

	_normalMap->Unlock();

	delete _normalMap;
}

void Terrain::BuildSector()
{
	chunkArray = new TerrainSection[int32(sectorCount.x * sectorCount.y)];
	//CreateSector();
}

void Terrain::CreateSector()
{
	for (int32 y = 0;y < sectorCount.y;y++)
	{
		for (int32 x = 0;x < sectorCount.x;x++)
		{
			uint32 heightMapX = x << chunkSizeShift;
			uint32 heightMapY = y << chunkSizeShift;

			int32 index = y * (int32)sectorCount.y + x;

			chunkArray[index].Create(
				this,
				x, y,
				heightMapX, heightMapY, 
				sectorVertexs, sectorSize,
				chunkSizeShift,
				gameHost);

			gameHost->PushNode(&chunkArray[index]);
		}
	}
}

//生成混合纹理
void Terrain::GenerateBlendImage(
	Texture* texture,
	ElevationData* data,
	int32 elevationDataCount)
{
	if (elevationDataCount == 0 || !data || !texture)
		return;

	elevationDataCount = Min(elevationDataCount, 4);

	int32 _width = texture->GetWidth();
	int32 _height = texture->GetHeight();

	ColorF color = 0;
	Vector4 channelMask[4];
	channelMask[0] = Vector4(1, 0, 0, 0);
	channelMask[1] = Vector4(0, 1, 0, 0);
	channelMask[2] = Vector4(0, 0, 1, 0);
	channelMask[3] = Vector4(0, 0, 0, 1);

	// 计算每像素的RGBA值
	for (int32 y = 0;y < 20;y++)
	{
		for (int32 x = 0;x < 20;x++)
		{
			float32 height = ReadHeight(x, y) * 1000.0F;
			Vector3 normal = ReadNormal(x, y);
			float32 totalScale = 0;

			// 存储4个通道的值
			Vector4 colorChannel = Vector4::zero();

			// 计算法线，计算高度图,遍历通道
			// 最多4个通道，正常3个通道
			for (int32 i = 0;i < elevationDataCount;i++)
			{
				float32 heightWeight = ComputeWeight(
					height,
					data[i].MinHeightZ, data[i].MaxHeightZ);

				//float32 normalWeight = ComputeWeight(
				//	normal.z,
				//	data[i].MinNormalZ, data[i].MaxNormalZ);

				float32 scale = heightWeight * data[i].Strength;
				colorChannel += (channelMask[i] * scale);

				totalScale += scale;
			}

			// 限制在 255 之内
			if (totalScale != 0)
			{
				float32 blendScale = 255.0F / totalScale;
				colorChannel *= blendScale;
			}

			//Debug::LogError("POS=", colorChannel);
			//Debug::LogError("TV="+Convert(colorChannel.x+ colorChannel.y+ colorChannel.z));

			uint8 r = uint8(Clamp(colorChannel.x, 0, 255.0F));
			uint8 g = uint8(Clamp(colorChannel.y, 0, 255.0F));
			uint8 b = uint8(Clamp(colorChannel.z, 0, 255.0F));
			uint8 a = uint8(Clamp(colorChannel.w, 0, 255.0F));
	 
		/*	Debug::LogError("R=" + Convert(r));
			Debug::LogError("G=" + Convert(g));
			Debug::LogError("B=" + Convert(b));
			Debug::LogError("A=" + Convert(a));*/

			ColorU32 coloe32(a, r, g, b);
			//ColorU32 red(0, 0, 199, 255);
			//uint32 color = red.GetColor();

			texture->SetColor(x, y, coloe32.GetColor());
		}
	}

}

Vector3& Terrain::ReadNormal(uint32 x, uint32 y)
{
	if (x >= heightMapWidth) { x = heightMapWidth - 1; }
	if (y >= heightMapHeight) { y = heightMapHeight - 1; }

	return normalMap[y * heightMapWidth + x];
}

float32& Terrain::ReadHeight(uint32 x, uint32 y)
{
	if (x >= heightMapWidth) x = heightMapWidth - 1;
	if (y >= heightMapHeight) y = heightMapHeight - 1;

	int32 index = y * heightMapWidth + x;
	return heightMap[index];
}

float32& Terrain::ReadHeight(int32 index)
{
	return heightMap[index];
}

// 当前高度越高,权重越低
float32 Terrain::ComputeWeight(
	float32 value,float32 minValue,float32 maxValue)
{
	float32 weight = 0.0F;
	if (value <= maxValue && value >= minValue)
	{
		float32 span = maxValue - minValue;
		weight = value - minValue;

		weight *= 1.0F / span;

		weight = 1.0F - AbsF(weight);
		weight = Clamp(weight, 0.001F, 1.0F);
	}
	return weight;
}


uint32 Terrain::GetMapIndex(uint16 mapX, uint16 mapY)
{
	if (mapX >= heightMapWidth)  mapX = heightMapWidth - 1;
	if (mapY >= heightMapHeight) mapY = heightMapHeight - 1;

	return (mapY * heightMapWidth) + mapX;
}

uint32 Terrain::GetSectionCount()const
{
	return uint32(sectorCount.x * sectorCount.y);
}

uint32 Terrain::GetSectionXCount()const
{
	return uint32(sectorCount.x);
}

uint32 Terrain::GetSectionYCount()const
{
	return uint32(sectorCount.y);
}

void Terrain::SetTexture(Texture* texture)
{
	int32 length = uint32(sectorCount.x * sectorCount.y);

	for (int32 i = 0;i < length;i++)
	{
		vector<MeshBuffer*> meshBuffers= chunkArray[i].GetMeshBuffers();

		for (uint32 j = 0;j < meshBuffers.size();j++)
		{
			meshBuffers[j]->SetTexture(texture);
		}
	}
}

void Terrain::SetMaterial(lMaterial* material)
{
	int32 length = int32(sectorCount.x * sectorCount.y);

	for (int32 i = 0;i < length;i++)
	{
		vector<MeshBuffer*> meshBuffers = chunkArray[i].GetMeshBuffers();

		for (uint32 j = 0;j < meshBuffers.size();j++)
		{
			meshBuffers[j]->SetMaterial(material);
		}
	}
}

void Terrain::SetTextureMulColor(bool _textureMulColor)
{
	int32 length = int32(sectorCount.x * sectorCount.y);
	
	for (int32 i = 0;i < length;i++)
	{
		chunkArray[i].SetTextureMulColor(_textureMulColor);
	}
}

void Terrain::SetRenderState(RENDER_STATE_TYPE State, uint32 value)
{
	int32 length = int32(sectorCount.x * sectorCount.y);

	for (int32 i = 0;i < length;i++)
	{
		vector<MeshBuffer*> meshBuffers = chunkArray[i].GetMeshBuffers();

		for (uint32 j = 0;j < meshBuffers.size();j++)
		{
			meshBuffers[j]->SetRenderState(State, value);
		}
	}
}

void Terrain::SetShaderType(SHADER_TYPE type)
{
	int32 length = int32(sectorCount.x * sectorCount.y);
	for (int32 i = 0;i < length;i++)
	{
		chunkArray[i].SetShaderType(type);
	}
}

void Terrain::SetHeightScale(float32 scale)
{
	int32 length = int32(sectorCount.x * sectorCount.y);
	for (int32 i = 0;i < length;i++)
	{
		chunkArray[i].SetHeightScale(scale);
	}
}

void Terrain::Destroy()
{
	SafeDelete(box);
	SafeDeleteArray(heightMap);
	SafeDeleteArray(normalMap);
}