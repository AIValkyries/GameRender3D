#include "MeshBuffer.h"
#include "Debug.h"
#include "UtilTools.h"

#define FACE_COUNT 1

MeshBuffer::MeshBuffer()
{
	name = "Mesh_" + Convert(MeshID);
	sourceBuffer = 0;
	transfromBuffer = 0;
	indexBuffer = 0;
	material = 0;
	aabb = 0;
	for (int32 i = 0;i < 4;i++)
	{
		texture[i] = 0;
	}

	faceStates = 0;
}

MeshBuffer::MeshBuffer(string _name)
{
	name = _name;
	sourceBuffer = 0;
	transfromBuffer = 0;
	indexBuffer = 0;
	material = 0;
	aabb = 0;
	for (int32 i = 0;i < 4;i++)
	{
		texture[i] = 0;
	}

	faceStates = 0;
}

void MeshBuffer::CreateVertex(int32 count)
{
	vertexCount = count;
	sourceCount = count;
	int32 prepVertexCount = FACE_COUNT * 3;   	// 预留5个面的顶点
	maxVertexCount = vertexCount + prepVertexCount;
	sourceBuffer = new Vertex[count + prepVertexCount];
	transfromBuffer = new Vertex[count + prepVertexCount];
}

void MeshBuffer::ResetVertexCount()
{
	vector<uint32>::iterator itr;

	for (itr = tempIndexs.begin();itr != tempIndexs.end();itr++)
	{
		indexBuffer->Remove(*itr);
	}
	tempIndexs.clear();
	vertexCount = sourceCount;
}

void MeshBuffer::InsertTransformFace(Vertex* faceVertexs)
{
	if (vertexCount >= maxVertexCount)
	{
		int32 prepVertexCount = FACE_COUNT * 3;   	// 预留5个面的顶点
		maxVertexCount += prepVertexCount;
		Vertex* transformVertexs = new Vertex[vertexCount + prepVertexCount];
		memcpy(transformVertexs, transfromBuffer, sizeof(Vertex) * vertexCount);
		delete[] transfromBuffer;
		transfromBuffer = transformVertexs;
	}
	int32 v0 = vertexCount++;
	int32 v1 = vertexCount++;
	int32 v2 = vertexCount++;

	transfromBuffer[v0] = faceVertexs[0];
	transfromBuffer[v1] = faceVertexs[1];
	transfromBuffer[v2] = faceVertexs[2];

	tempIndexs.push_back(v0);
	tempIndexs.push_back(v1);
	tempIndexs.push_back(v2);

	// 插入一个三角形索引
	indexBuffer->PushBack(v0);
	indexBuffer->PushBack(v1);
	indexBuffer->PushBack(v2);

	//Debug::LogInfo("插入三角面=(" 
	//	+ Convert(v0) + 
	//	"," + Convert(v1) + "," 
	//	+ Convert(v2) + ")");
}


void MeshBuffer::SetTransformVertex(Vertex* vertexs)
{
	if (transfromBuffer) delete[] transfromBuffer;
	transfromBuffer = vertexs;
}
 void MeshBuffer::SetTransformVertex(int32 index, Vertex& vertex)
 {
	 if (uint32(index) >= vertexCount)
		 Debug::LogError("SetTransformVertex=顶点索引错误!");
	 transfromBuffer[index] = vertex;
 }
 void MeshBuffer::SetSourceVertex(Vertex* vertexs)
 {
	 if (sourceBuffer) delete[] sourceBuffer;
	 sourceBuffer = vertexs;
 }
 void MeshBuffer::SetSourceVertex(int32 index, Vertex& vertex)
 {
	 if (uint32(index) >= vertexCount)
		 Debug::LogError("SetSourceVertex=顶点索引错误!");
	 vertex.Index = index;
	 sourceBuffer[index] = vertex;
 }
 Vertex* MeshBuffer::GetTransformVertex(int32 index)
 {
	 if (uint32(index) >= vertexCount)
	 {
		 Debug::LogError("GetTransformVertex=顶点索引错误=" + Convert(index));
	 }
	 return &transfromBuffer[index];
 }
 Vertex* MeshBuffer::GetSourceVertex(int32 index)
 {
	 if (uint32(index) >= vertexCount)
	 {
		 Debug::LogError("GetSourceVertex=顶点索引错误=" + Convert(index));
	 }
	 return &sourceBuffer[index];
 }

 int32 MeshBuffer::GetVertexCount()
 {
	 return vertexCount;
 }

 void MeshBuffer::CreateIndices(INDEX_TYPE type, int32 size)
 {
	 indexBuffer = new IndexBuffer(type);
	 for (int32 i = 0;i < size;i++)
	 {
		 indexBuffer->PushBack(-1);
	 }
	 CreateFaceState();
 }

 void MeshBuffer::SetIndices(IndexBuffer* _indexBuffer)
 {
	 if (indexBuffer) delete indexBuffer;
	 indexBuffer = _indexBuffer;
 }

 void MeshBuffer::SetIndices(uint32 index, uint32 value)
 {
	 indexBuffer->SetValue(index, value);
 }

 void MeshBuffer::ClearIndices()
 {
	 if (indexBuffer == 0)
		 return;
	 indexBuffer->Clear();
 }

 void MeshBuffer::PushIndices(uint32& value)
 {
	 if (indexBuffer == 0)
		 indexBuffer = new IndexBuffer(INDEX_16BIT);
	 indexBuffer->PushBack(value);
 }

 IndexBuffer* MeshBuffer::GetIndexBuffer()
 {
	 return indexBuffer;
 }

 lMaterial* MeshBuffer::GetMaterial()
 {
	 return material;
 }

 Texture* MeshBuffer::GetTexture()
 {
	 return texture[0];
 }

 Texture* MeshBuffer::GetTexture(int32 index)
 {
	 if (index >= MAX_TEXTURE_COUNT)
		 return 0;
	 return texture[index];
 }

 void MeshBuffer::SetMaterial(lMaterial* _material)
 {
	 if (material)  delete material;
	 material = _material;
 }

 void MeshBuffer::SetTexture(Texture* _texture)
 {
	 if (textureIndex >= MAX_TEXTURE_COUNT)
		 return;
	 texture[textureIndex++] = _texture;
 }

 AABB* MeshBuffer::GetAABB()
 {
	 return aabb;
 }

 void MeshBuffer::ResetAABB(float32 x, float32 y, float32 z)
 {
	 if (aabb)  delete aabb;

	 aabb = new AABB();
	 aabb->Set(x, y, z, x, y, z);
 }

 void MeshBuffer::SetAABB(AABB* box)
 {
	 if (aabb)  delete aabb;
	 aabb = box;
 }

 AABB* MeshBuffer::RecalculateAABB()
 {
	 aabb->Set(sourceBuffer[0].position);
	 for (uint32 i = 1;i < sourceCount;i++)
	 {
		 aabb->UnionPoint(sourceBuffer[i].position);
	 }
	 return aabb;
 }

 void MeshBuffer::SetRenderState(RENDER_STATE_TYPE State, uint32 value)
 {
	 renderState[State] = value;
 }

 bool MeshBuffer::CheckRenderState(RENDER_STATE_TYPE State)
 {
	 return renderState[State] != 0;
 }

 void MeshBuffer::CreateFaceState()
 {
	 uint32 primitiveCount = indexBuffer->GetPrimitiveCount();
	 faceStates = new uint32[primitiveCount];

	 for (uint32 i = 0;i < primitiveCount;i++)
	 {
		 ClearFace(i);
	 }
 }

 void MeshBuffer::SetFaceState(uint32 faceID, uint32 value)
 {
	 if (!faceStates)
		 return;

	 faceStates[faceID] |= value;
 }

 bool MeshBuffer::CheckFaceState(uint32 faceID, uint32 value)
 {
	 if (!faceStates)return false;
	 if (faceStates[faceID] & value)
	 {
		 return true;
	 }
	 return false;
 }

 void MeshBuffer::ClearFaceState(uint32 faceID, uint32 value)
 {
	 if (!faceStates)return;
	 faceStates[faceID] &= (~value);
 }

 void MeshBuffer::ClearFace(uint32 faceID)
 {
	 if (!faceStates)return;
	 faceStates[faceID] = 0;
 }


 void MeshBuffer::Destroy()
 {
	 SafeDeleteArray(sourceBuffer);
	 SafeDeleteArray(transfromBuffer);
	 SafeDelete(indexBuffer);
	 SafeDelete(material);
	 SafeDelete(aabb);

	 //for (int32 i = 0;i < 4;i++)
	 //{
		// SafeDelete(texture[i]);
	 //}
	 SafeDeleteArray(faceStates);
 }