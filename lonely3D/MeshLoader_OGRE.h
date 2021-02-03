#pragma once
#ifndef __MeshLoader_OGRE_H__
#define __MeshLoader_OGRE_H__

#include "IResourceLoader.h"
#include "IndexBuffer.h"
#include "BoneAssignment.h"
#include "Animation.h"
#include "Bone.h"
#include "KeyFrame.h"
#include "OgreMeshFileFormat.h"
#include "Color.h"
#include "SkinnedSkeleton.h"

#pragma pack(1)  //�趨Ϊ 1 �ֽڶ���
struct ChunkHeader
{
	ChunkHeader() :ID(0), Length(0) {}
	uint16 ID;
	uint32 Length;
};

struct ChunkData
{
	ChunkData():Read(0) {}
	ChunkHeader Header;
	uint32 Read;
};

//{
//	0, //stream index
//		0,    // �ֶ�ƫ�ƣ����ֽ�Ϊ��λ
//		D3DDECLTYPE_FLOAT2,    // ��������
//		D3DDECLMETHOD_DEFAULT, // ��������
//		D3DDECLUSAGE_POSITION, // ���壬�������
//		0                      // ��ͬ����Ķ��������index
//}
//{
//	0,
//		8,                     // ������ֶ�Ϊfloat2������4*2=8
//		D3DDECLTYPE_FLOAT2,
//		D3DDECLMETHOD_DEFAULT,
//		D3DDECLUSAGE_TEXCOORD,
//		0
//}

// |VertexDeclaration|,|VertexBuffer|,|VertexBuffer|,|...|
struct OgreVertexDeclaration
{
	uint16 Source;    // buffer bind source
	uint16 Type;
	uint16 Semantic;  //����Pos(1),Normal(4),TexCoord(7)
	uint16 Offset;    // �������е���ʼƫ���������ֽ�Ϊ��λ��
					  // �����������������ɫ���������꣩
	uint16 Index;
};

struct OgreVertexBuffer
{
	// ��OgreVertexElement�е�Source
	uint16 BindIndex;
	//ÿ�����С���������������������һ��
	uint16 VertexSize;
	float32* Data;
};

struct OgreGeometry
{
	int32 VertexCount;
	vector<OgreVertexDeclaration*> Elements;
	vector<OgreVertexBuffer*> Buffers;
};

struct OgreSubMesh
{
	OgreGeometry* Geometry;
	IndexBuffer* Indices;
	int32 IndexCount;
	uint16 Operation;    // ������list������������?? ��ʱ��֧����������
	vector<BoneAssignment*> Assignment;
	string MaterialName;
	string TextureName;
	AABB* aabb;
};

struct OgreMaterial
{
	bool EnableZBuffer;
	bool DepthWrite;
	bool EnableNormal;
	bool EnableLight;
	int32 RenderMode;

	//����
	string TextureName;
	WARP_MODE WarpMode;
	FILTER_MODE FilterMode;
	FILE_ACCESS FileAccess;
};

class Vector3;
class Quaternion;
class GameHost;

class MeshLoader_OGRE:public IResourceLoader
{
public:
	virtual bool IsALoadableFileExtension(const string& fileName);
	virtual bool Loader(IFile* file, IResourceItem* item);
	virtual void SetGameHost(GameHost* _gameHost);
private:
	bool swapEndian;
	vector<OgreSubMesh*> subMeshs;
	vector<OgreMaterial*> materials;
	vector<BoneAssignment*> boneAssignments;

	SkinnedSkeleton* skeleton;
	
	int32 numUV;

	void ComposeObject();
	void BuildBufferPosition(
		vector<OgreVertexBuffer*> buffers,
		OgreVertexDeclaration* element,
		MeshBuffer* meshBuffer,
		int32 vertexCount);

	void BuildBufferNormal(
		vector<OgreVertexBuffer*> buffers,
		OgreVertexDeclaration* element,
		MeshBuffer* meshBuffer,
		int32 vertexCount);

	void BuildBufferTexCoord(
		vector<OgreVertexBuffer*> buffers, 
		OgreVertexDeclaration* element,
		MeshBuffer* meshBuffer,
		int32 vertexCount);

	/// <summary>
	/// LoadMesh��LoadSkeleton��LoadMaterials
	/// </summary>
	bool LoadMesh(IFile* file,ChunkData& data);

	void ReadMesh(IFile* file,ChunkData& parent);
	void ReadSubMesh(IFile* file,ChunkData& parent, OgreSubMesh* subMesh);
	void ReadGeometry(IFile* file, ChunkData& parent, OgreGeometry* geometry);
	void ReadVertexDeclaration(IFile* file,ChunkData& parent, OgreGeometry* geometry);
	void ReadVertexBuffer(IFile* file, ChunkData& parent, OgreGeometry* geometry);

	void LoadSkeleton(string& fileName);

	void LoadMaterials(OgreSubMesh* subMesh);

	void GetMaterialToken(IFile* file, string& token);
	void ReadTechnique(IFile* file, OgreSubMesh* subMesh,const string& materialName);
	void ReadPass(IFile* file, OgreSubMesh* subMesh, const string& materialName);
	void ReadColor(IFile* file, ColorF& color);
	void ReadFloat32(IFile* file, float32& value);

	// Read ��Ϣ
	void ReadChunkData(IFile* file,ChunkData& data);
	void ReadString(IFile* file, ChunkData& data, string& out);
	void ReadBool(IFile* file, ChunkData& data, bool& out);
	void ReadInt32(IFile* file, ChunkData& data, int32& out);
	void ReadShort(IFile* file, ChunkData& data, uint16& out);
	void ReadFloat(IFile* file, ChunkData& data, float32& out);
	void ReadVector3D(IFile* file, ChunkData& data, Vector3& out);
	void ReadVector4D(IFile* file, ChunkData& data, Vector4& out);
	void ReadQuaternion(IFile* file, ChunkData& data, Quaternion& out);

	bool Isspace(char c);
	bool IsNumber(char c);
};

#endif