#pragma once
#ifndef __MeshLoader_X_H__
#define __MeshLoader_X_H__

#include "IResourceLoader.h"
#include "DataType.h"
#include "lMathf.h"
#include "Color.h"
#include "Vertex.h"
#include "IndexBuffer.h"
#include "Material.h"
#include "BoneAssignment.h"
#include "Texture.h"
#include <vector>
#include <map>
#include "SkinnedSkeleton.h"

struct XPositionKey
{
	uint32 Frame;
	Vector3 Position;
};

struct XScaleKey
{
	uint32 Frame;
	Vector3 Scale;
};

struct XRotationKey
{
	uint32 Frame;
	Quaternion Rotation;
};

struct XAnimation
{
	string Name;
	uint32 Length;
	string JointName;

	vector<XPositionKey*>  PositionKeys;
	vector<XScaleKey*>     ScaleKeys;
	vector<XRotationKey*>  RotationKeys;
};

struct XMesh
{
	// 顶点{Position,Normal,TextureCoords,Color}
	// 索引
	// 材质
	// 权重
	vector<Vertex*> Vertexs;
	IndexBuffer* Indices;
	int32* VertexDuplicationIndices;
	vector<string> TextureNames;

	vector<lMaterial*> Materials;
	map<lMaterial*, Texture*> MaterialsBindTexture;

	// 不同材质对应的索引集合
	vector<uint32> FaceMaterialIndices;
};

class MeshLoader_X: public IResourceLoader
{
public:
	virtual bool IsALoadableFileExtension(const string& fileName);
	virtual bool Loader(IFile* file, IResourceItem* item);
	virtual void SetGameHost(GameHost* _gameHost);

private:
	XMesh* mesh;
	SkinnedSkeleton* skeleton;

	char* p;
	char* end;
	char* buffer;

	bool binaryFormat;
	int8 floatSize;
	INDEX_TYPE indexType;

	bool ReadFileIntoMemory(IFile* file);  // 读取文件内容至缓存

	bool ParseFile();                      // 循环解析文件
	bool ReadFrame(Bone* parent);
	bool ReadTransformationMatrix(Matrix4x4& mat);
	bool ReadMesh(XMesh* mesh);

	bool ReadMeshNormals(XMesh* mesh);
	bool ReadMeshTextureCoords(XMesh* mesh);
	bool ReadMeshMaterialList(XMesh* mesh);
	bool ReadMaterial(XMesh* mesh);

	bool ReadXSkinMeshHeader(XMesh* mesh);
	bool ReadSkinWeights(XMesh* mesh);

	bool ReadAnimationSet();  // 解析动画
	bool ReadAnimation(Animation* animation);
	bool ReadAnimationKey(Animation* animation,Bone* bone);
	bool ReadAnimationOptions();

	bool ReadDataObjectEnd();            // 读取到'}'
	bool ReadDataObjectStart();
	bool ReadUntilEndOfLine();            //读取到行结束
	string GetToken();
	string GetAnimationName();
	string GetNextTokenAsString();

	bool FindNextNoneWhiteSpace();        // 读取空格

	bool ReadSemicolon();      // 读取分号
	bool ReadClosingBrace();   // 读取右括号
	bool ReadLeftBrace();      // 读取左括号
	bool ReadComma();          // 读取逗号

	float32   ReadFloat();
	int32     ReadInt32();
	Vector2   ReadVector2();
	Vector3   ReadVector3();
	Vector4   ReadVector4();
	Matrix4x4 ReadMatrix4x4();
	ColorF    ReadRGBA();
	ColorF    ReadRGB();

	bool Isspace(char c);
	bool IsFloat32(char c);
	bool IsInt32(char c);
};


#endif

