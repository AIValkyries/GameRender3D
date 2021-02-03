#include "TerrainSection.h"
#include "Vertex.h"
#include "Terrain.h"
#include "IndexBuffer.h"
#include "GameHost.h"
#include "Debug.h"

void TerrainSection::Create(
	Terrain* terrain,
	int32 _sectorX, int32 _sectorY,
	int32 _heightMapX, int32 _heightMapY,
	int32 _vertexCount, Vector2 _sectorSize,
	uint32 _chunkSizeShift,
	GameHost* _gameHost)
{
	SceneObject::Create(_gameHost);

	heightScale = Max(heightScale, 1);
	mainTerrain = terrain;
	sectorX = _sectorX;
	sectorY = _sectorY;
	heightMapX = _heightMapX;
	heightMapY = _heightMapY;

	vertexCount = _vertexCount;
	sectorSize = _sectorSize;

	chunkSizeShift = _chunkSizeShift;

	buffer = new MeshBuffer();

	BuildVertexBuffer();
	BuildIndexBuffer();

	AttachToQuadTree(gameHost->GetQuadTree());
}

/* 272------------------288
	  |                  |
	  |                  |
	  |                  |
	  |                  |
	 0 ------------------16
	 顶点范围
*/
void TerrainSection::BuildVertexBuffer()
{
	uint32 chunkMapSize = vertexCount - 1;

	int32 vertexSize = vertexCount * vertexCount;

	Vector2 cellSize(
		sectorSize.x / chunkMapSize,
		sectorSize.y / chunkMapSize);

	Debug::LogInfo("每步大小,X=" + Convert(cellSize.x) + ",Y=" + Convert(cellSize.y));

	Vertex* vertexs = new Vertex[vertexSize];

	float32 U = float32(mainTerrain->GetSectionXCount() * chunkMapSize);
	float32 V = float32(mainTerrain->GetSectionYCount() * chunkMapSize);

	Vector2 offsetPos(sectorX * sectorSize.x, sectorY * sectorSize.y);
	Vector2 offsetUV( float32(sectorX * chunkMapSize), float32(sectorY * chunkMapSize));
	Vector2 maxUV(U, V);

	buffer->ResetAABB(0, 0, 0);
	AABB* aabb = buffer->GetAABB();

	Vector4 minPos(999999, 999999, 999999, 999999);
	Vector4 maxPos(0, 0, 0, 0);

	uint32 mapHeight =  mainTerrain->GetHeightMapHeight();

	for (int32 y = 0;y < vertexCount;y++)
	{
		Vector2 step(offsetPos.x, y * cellSize.y + offsetPos.y);

		for (int32 x = 0;x < vertexCount;x++)
		{
			int32   index = y * vertexCount + x;

			// 高度图的起始点在左上角
			// 顶点位置的高度图在左下角
			int32 idX = (heightMapX + x);
			int32 idY = mapHeight - (heightMapY + y);

			float32 height = mainTerrain->ReadHeight(idX, idY);
			Vector3 normal = mainTerrain->ReadNormal(idX, idY);
			Vector4 position = Vector4(step.x, height * heightScale, step.y, 1);

			minPos = Min(minPos, position);
			maxPos = Max(maxPos, position);

			vertexs[index].position = position;
			vertexs[index].normal = normal;

			//Debug::LogInfo("-------------------------------------------");
			//Debug::LogInfo("INDEX=" + Convert(index));
			//Debug::LogInfo("POS=", position);

			Vector2 uv = Vector2(
				(x + offsetUV.x) / maxUV.x,
				(y + offsetUV.y) / maxUV.y);

			vertexs[index].uv = uv;
			step.x += cellSize.x;

			aabb->UnionPoint(position);
		}
	}

	worldBounds = new AABB();
	sourceBounds = new AABB();
	worldBounds->Set(aabb);
	sourceBounds->Set(aabb);

	Debug::LogWarning("-------------地形块 [X=" + Convert(sectorX) + ",Y=" + Convert(sectorY) + "]-----------------------------------");
	Debug::LogInfo("MIN_POSITION=" , minPos);
	Debug::LogInfo("MAX_POSITION=", maxPos);
	Debug::LogInfo("POS_OFFSET=", offsetPos);
	Debug::LogInfo("UV_OFFSET=", offsetUV);
	Debug::LogInfo("BOX_MIN=", aabb->min);
	Debug::LogInfo("BOX_MAX=", aabb->max);
	Debug::LogInfo("顶点数量=" + Convert(vertexSize));

	buffer->CreateVertex(vertexSize);
	buffer->SetSourceVertex(vertexs);

	Vertex* transVertexs = new Vertex[vertexSize];
	std::memcpy(transVertexs, vertexs, sizeof(Vertex) * vertexSize);

	buffer->SetTransformVertex(transVertexs);

	meshBuffers.push_back(buffer);
}

/*X0 0------------------2 X1
     |                  |
     |                  |
     |                  |
     |                  |
  Y0 1------------------3 Y1
*/
void TerrainSection::BuildIndexBuffer()
{
	IndexBuffer* indexBuffer = new IndexBuffer(INDEX_16BIT);

	int32 total_strips = vertexCount - 1;
	uint32 start_vert = 0;

	int32 xStep = 1;
	int32 yStep = 1;

	int32 index = 0;
	int32 lineStep = yStep * vertexCount;

	int32 indexCount = 0;

	for (int32 y = 0;y < total_strips;y++)
	{
		uint32 vert = start_vert;
		vert += xStep;

		for (int32 x = 1;x < vertexCount;x++)
		{
			uint32 vertX0 = vert - 1;
			uint32 vertX1 = vert;

			uint32 vertY1 = vert + lineStep;
			uint32 vertY0 = vertY1 - 1;

			//Debug::LogInfo(
			//	"X0=" + Convert(vertX0) +
			//	",X1=" + Convert(vertX1) +
			//	",Y0=" + Convert(vertY0) +
			//	",Y1=" + Convert(vertY1));

			indexBuffer->PushBack(vertY0);
			indexBuffer->PushBack(vertX0);
			indexBuffer->PushBack(vertX1);

			indexBuffer->PushBack(vertX1);
			indexBuffer->PushBack(vertY1);
			indexBuffer->PushBack(vertY0);

			indexCount += 6;

			vert += xStep;
		}
		start_vert += lineStep;
	}

	Debug::LogInfo("索引长度=" + Convert(indexCount));
	buffer->SetIndices(indexBuffer);

}

void TerrainSection::Render(bool updateChild)
{
	SceneObject::Render(updateChild);
}

void TerrainSection::UpdateBox(bool updateChild)
{
	if (worldBounds && sourceBounds)
	{
		worldBounds->Set(sourceBounds);
		TransformBox(localToWorldMatrix, *worldBounds);
	}
	SceneObject::UpdateBox(updateChild);
}

void TerrainSection::Update(bool updateChild)
{
	SceneObject::Update(updateChild);
}

void TerrainSection::Destroy()
{
	buffer->Destroy();
	buffer = 0;

	SceneObject::Destroy();
}

// 提取地形块的 4个角的索引，索引=高度图的索引
void TerrainSection::ExtractFourCornersIndex(
	uint32& index0, uint32& index1,
	uint32& index2, uint32& index3,
	uint32& x0, uint32& y0,
	uint32& x1, uint32& y1)
{
	x0 = heightMapX;
	y0 = heightMapY;

	x1 = x0 + (1 << chunkSizeShift);
	y1 = y0 + (1 << chunkSizeShift);

	index0 = mainTerrain->GetMapIndex(uint16(x0), uint16(y0));
	index1 = mainTerrain->GetMapIndex(uint16(x0), uint16(y1));
	index2 = mainTerrain->GetMapIndex(uint16(x1), uint16(y1));
	index3 = mainTerrain->GetMapIndex(uint16(x1), uint16(y0));
}

Vector4& TerrainSection::ReadPosition(int32 index)
{
	return 	buffer->GetTransformVertex(index)->position;
}

void TerrainSection::SetHeightScale(float32 scale)
{
	heightScale = scale;
}