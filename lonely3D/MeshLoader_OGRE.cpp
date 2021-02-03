#include "MeshLoader_OGRE.h"
#include "UtilTools.h"
#include "OgreMeshFileFormat.h"
#include "Vector3.h"
#include "Quaternion.h"
#include "Debug.h"
#include "FIleInfo.h"
#include "GameHost.h"

void MeshLoader_OGRE::SetGameHost(GameHost* _gameHost)
{
	gameHost = _gameHost;
}

bool MeshLoader_OGRE::IsALoadableFileExtension(const string& fileName)
{
	return IsFileExtension(fileName, "mesh");
}

bool MeshLoader_OGRE::Loader(IFile* file, IResourceItem* item)
{
	uint16 id;
	file->Read(&id, 2);

	if (id == OGRE_HEADER)
		swapEndian = false;
	else if (id == 0x0010)
		swapEndian = true;
	else
		return 0;

	skeleton = (SkinnedSkeleton*)item;

	string version;
	ChunkData data;
	ReadString(file, data, version);

	if (LoadMesh(file, data))
	{

	}

	Debug::LogInfo("---------------------------��ʼ��ȡ����---------------------------");

	// ���ز���
	for (uint32 i = 0;i < subMeshs.size();i++)
	{
		LoadMaterials(subMeshs[i]);
	}
	
	Debug::LogInfo("��ȡ���!!!");

	ComposeObject();
	skeleton->Finalize();

	if (subMeshs.size())
	{
		for (uint32 i = 0;i < subMeshs.size();i++)
		{
			delete subMeshs[i];
		}
		subMeshs.clear();
	}

	if (materials.size())
	{
		for (uint32 i = 0;i < materials.size();i++)
		{
			delete materials[i];
		}
		materials.clear();
	}
	boneAssignments.clear();

	Debug::LogInfo("��Ƥ�����������!!");

	return true;
}

// ��ʼ������Ƥ����
void MeshLoader_OGRE::ComposeObject()
{
	for (uint32 i = 0;i < subMeshs.size();i++)
	{
		MeshBuffer* meshBuffer = skeleton->AddMeshBuffer();

		if (subMeshs[i]->TextureName.size() != 0)
		{
			const string textureFilePath = "../media/texture/" + subMeshs[i]->TextureName;
			Texture* texture = gameHost->GetTextureManager()->LoadResource(textureFilePath);
			meshBuffer->SetTexture(texture);
		}
		OgreGeometry* geometry = subMeshs[i]->Geometry;

		meshBuffer->SetIndices(subMeshs[i]->Indices);
		meshBuffer->CreateVertex(geometry->VertexCount);
		meshBuffer->SetAABB(subMeshs[i]->aabb);

		// ��������buffer
		for (uint32 j = 0;j < geometry->Elements.size();j++)
		{
			if (geometry->Elements[j]->Semantic == 1)            // POS
			{
				BuildBufferPosition(
					geometry->Buffers, 
					geometry->Elements[j], 
					meshBuffer, 
					geometry->VertexCount);
			}
			else if (geometry->Elements[j]->Semantic == 4)      // NORMAL
			{
				BuildBufferNormal(
					geometry->Buffers, 
					geometry->Elements[j],
					meshBuffer, 
					geometry->VertexCount);
			}
			else if (geometry->Elements[j]->Semantic == 7)      // TexCoord
			{
				BuildBufferTexCoord(
					geometry->Buffers,
					geometry->Elements[j],
					meshBuffer, 
					geometry->VertexCount);
			}
		}
	}

	vector<Bone*> boneAll = skeleton->GetAllBones();
	vector<MeshBuffer*> meshBuffers = skeleton->GetSkinMeshBuffer();

	for (uint32 i = 0;i < boneAssignments.size();i++)
	{
		BoneAssignment* boneA = boneAssignments[i];

		MeshBuffer* buffer  = meshBuffers[boneA->BufferID];
		boneA->StaticPos    = buffer->GetSourceVertex(boneA->VertexID)->position;
		boneA->StaticNormal = buffer->GetSourceVertex(boneA->VertexID)->normal;
		
		Bone* bone = boneAll[boneA->BoneID];
		bone->Weights.push_back(boneA);
	}

	for (int32 i = boneAll.size() - 1;i >= 0;i--)
	{
		Bone* bone = boneAll[i];

		bone->Rotate = Quaternion::identity();
		bone->Position = Vector4::zero();
		bone->Scale = Vector3::zero();

		if (bone->Parent != 0xFFFF)
		{
			Bone* parent = boneAll[bone->Parent];
			parent->Children.push_back(bone);
		}
	}

}

void MeshLoader_OGRE::BuildBufferPosition(
	vector<OgreVertexBuffer*> buffers, 
	OgreVertexDeclaration* element,
	MeshBuffer* meshBuffer,
	int32 vertexCount)
{
	for (uint32 i = 0;i < buffers.size();i++)
	{
		if (buffers[i]->BindIndex == element->Source)
		{
			uint32 size = buffers[i]->VertexSize;
			uint32 pos = element->Offset;

			for (int32 k = 0;k < vertexCount;k++)
			{
				float32 x = buffers[i]->Data[pos];
				float32 y = buffers[i]->Data[pos + 1];
				float32 z = -buffers[i]->Data[pos + 2];

				meshBuffer->GetSourceVertex(k)->position.Set(x, y, z, 1);
				pos += size;
			}
		}
	}
}

void MeshLoader_OGRE::BuildBufferNormal(
	vector<OgreVertexBuffer*> buffers,
	OgreVertexDeclaration* element,
	MeshBuffer* meshBuffer,
	int32 vertexCount)
{
	for (uint32 i = 0;i < buffers.size();i++)
	{
		if (buffers[i]->BindIndex == element->Source)
		{
			uint32 size = buffers[i]->VertexSize;
			uint32 pos = element->Offset;

			for (int32 k = 0;k < vertexCount;k++)
			{
				meshBuffer->GetSourceVertex(k)->normal.Set(
					buffers[i]->Data[pos],
					buffers[i]->Data[pos + 1],
					-buffers[i]->Data[pos + 2]);
				pos += size;
			}
		}
	}
}

void MeshLoader_OGRE::BuildBufferTexCoord(
	vector<OgreVertexBuffer*> buffers, 
	OgreVertexDeclaration* element,
	MeshBuffer* meshBuffer,
	int32 vertexCount)
{
	for (uint32 i = 0;i < buffers.size();i++)
	{
		if (buffers[i]->BindIndex == element->Source)
		{
			uint32 size = buffers[i]->VertexSize;
			uint32 pos = element->Offset;

			for (int32 k = 0;k < vertexCount;k++)
			{
				meshBuffer->GetSourceVertex(k)->uv.Set(
					buffers[i]->Data[pos],
					buffers[i]->Data[pos + 1]);
				pos += size;
			}
		}
	}

}

bool MeshLoader_OGRE::LoadMesh(IFile* file, ChunkData& parent)
{
	while (file->GetPos() < file->GetFileSize())
	{
		ChunkData data;
		ReadChunkData(file, data);

		switch (data.Header.ID)
		{
		case OGRE_MESH:
		{
			ReadMesh(file, data);
		}break;
		}
	}

	return true;
}

void MeshLoader_OGRE::ReadMesh(IFile* file, ChunkData& parent)
{
	bool skeletallyAnimated;
	ReadBool(file, parent, skeletallyAnimated);
	Debug::LogInfo("�Ƿ�����Ƥ����:" + Convert((int32)(skeletallyAnimated ? true : false)));

	string name;
	bool skeleton_loaded = false;

	Vector3 minBoxPoint;
	Vector3 maxBoxPoint;
	float32 boxRadius;
	OgreSubMesh* subMesh = 0;

	while (parent.Read < parent.Header.Length)
	{
		ChunkData data;
		ReadChunkData(file, data);

		switch (data.Header.ID)
		{
		case OGRE_SUBMESH:  // ��mesh
		{
			subMesh = new OgreSubMesh();
			subMeshs.push_back(subMesh);
			ReadSubMesh(file, data, subMesh);
		}break;
		case OGRE_MESH_SKELETON_LINK:  // ��Ƥ����
		{
			ReadString(file, data, name);
			LoadSkeleton(name);
			skeleton_loaded = true;
		}break;
		case OGRE_MESH_BOUNDS:            // bounds,box
		{
			ReadVector3D(file, data, minBoxPoint);
			ReadVector3D(file, data, maxBoxPoint);
			ReadFloat(file, data, boxRadius);

			if (subMesh)
			{
				if (subMesh->aabb == 0)
					subMesh->aabb = new AABB();
				subMesh->aabb->UnionPoint(minBoxPoint);
				subMesh->aabb->UnionPoint(maxBoxPoint);
			}
		}break;
		// ignore chunk
		case OGRE_MESH_BONE_ASSIGNMENT:
		case OGRE_MESH_LOD_LEVEL:
		case OGRE_SUBMESH_NAME_TABLE:
		case OGRE_EDGE_LISTS:
		case OGRE_POSES:
		case OGRE_ANIMATIONS:
		{
			int32 length = data.Header.Length - data.Read;
			file->Seek(length, SEEK_CUR);
			data.Read += length;
		}break;
		default:
		{
			int32 length = data.Header.Length - data.Read;
			file->Seek(length, SEEK_CUR);
			data.Read += length;
		}
		break;
		}

		parent.Read += data.Read;
	}

	if (!skeleton_loaded)
		LoadSkeleton(name);
}

void MeshLoader_OGRE::ReadSubMesh(IFile* file, ChunkData& parent, OgreSubMesh* subMesh)
{
	ReadString(file, parent, subMesh->MaterialName);
	Debug::LogInfo("��������:" + subMesh->MaterialName);

	bool useSharedVertices;
	ReadBool(file, parent, useSharedVertices);

	ReadInt32(file, parent, subMesh->IndexCount);

	Debug::LogInfo("��������:" + subMesh->IndexCount);

	bool indexes32Bit;
	ReadBool(file, parent, indexes32Bit);
	
	if (indexes32Bit)
	{
		subMesh->Indices = new IndexBuffer(INDEX_32BIT);
		
		for (int32 i = 0;i < subMesh->IndexCount;i++)
		{
			int32 num;
			ReadInt32(file, parent, num);
			subMesh->Indices->PushBack(num);
		}
	}
	else
	{
		subMesh->Indices = new IndexBuffer(INDEX_16BIT);

		for (int32 i = 0;i < subMesh->IndexCount;i++)
		{
			uint16 num;
			ReadShort(file, parent, num);
			subMesh->Indices->PushBack(num);
		}
	}

	while (parent.Read < parent.Header.Length)
	{
		ChunkData data;
		ReadChunkData(file, data);
		
		switch (data.Header.ID)
		{
		case OGRE_GEOMETRY:
		{
			if (!subMesh->Geometry)
				subMesh->Geometry = new OgreGeometry();
			ReadGeometry(file, data, subMesh->Geometry);
		}break;
		case OGRE_SUBMESH_OPERATION:
		{
			ReadShort(file, data, subMesh->Operation);
		}break;
		case OGRE_SUBMESH_BONE_ASSIGNMENT:
		{
			BoneAssignment* boneA = new BoneAssignment();

			ReadInt32(file, data, boneA->VertexID);
			ReadShort(file, data, boneA->BoneID);
			ReadFloat(file, data, boneA->Weight);

			boneAssignments.push_back(boneA);
		}break;
		case OGRE_SUBMESH_TEXTURE_ALIAS:
		{
			string aliasName;
			string textureName;
			ReadString(file, data, aliasName);
			ReadString(file, data, textureName);
		}break;
		default: 
		{
			int32 length = data.Header.Length - data.Read;
			file->Seek(length, SEEK_CUR);
			data.Read += length;
		}break;
		}

		parent.Read += data.Read;
	}

}

void MeshLoader_OGRE::ReadGeometry(
	IFile* file, ChunkData& parent, OgreGeometry* geometry)
{
	ReadInt32(file, parent, geometry->VertexCount);

	while (parent.Read < parent.Header.Length)
	{
		ChunkData data;
		ReadChunkData(file, data);

		// |VertexDeclaration|,|VertexBuffer|,|VertexBuffer|,|...|
		switch (data.Header.ID)
		{
		case OGRE_GEOMETRY_VERTEX_DECLARATION:
		{
			ReadVertexDeclaration(file, data, geometry);
		}break;
		case OGRE_GEOMETRY_VERTEX_BUFFER:
		{
			ReadVertexBuffer(file, data, geometry);
		}break;
		default:
		{
			int32 length = data.Header.Length - data.Read;
			file->Seek(length, SEEK_CUR);
			data.Read += length;
		}break;
		}

		parent.Read += data.Read;
	}

	if (parent.Read != parent.Header.Length)
		Debug::LogError("Incorrect geometry length. File might be corrupted.");
}

void MeshLoader_OGRE::ReadVertexDeclaration(
	IFile* file, ChunkData& parent, OgreGeometry* geometry)
{
	while (parent.Read < parent.Header.Length)
	{
		ChunkData data;
		ReadChunkData(file, data);

		switch (data.Header.ID)
		{
		case OGRE_GEOMETRY_VERTEX_ELEMENT:
		{
			OgreVertexDeclaration* declaration = new OgreVertexDeclaration();
			geometry->Elements.push_back(declaration);

			ReadShort(file, data, declaration->Source);
			ReadShort(file, data, declaration->Type);
			// ����Semantic=Pos(1), Normal(4), TexCoord(7)
			ReadShort(file, data, declaration->Semantic);
			// �������е���ʼƫ���������ֽ�Ϊ��λ��
			ReadShort(file, data, declaration->Offset);
			declaration->Offset /= sizeof(float32);
			ReadShort(file, data, declaration->Index);

			if (declaration->Semantic == 7)
			{
				numUV++;
			}

		}break;
		default:
		{
			int32 length = data.Header.Length - data.Read;
			file->Seek(length, SEEK_CUR);
			data.Read += length;
		}break;
		}

		parent.Read += data.Read;
	}

	if (parent.Read != parent.Header.Length)
		Debug::LogError("ReadVertexDeclaration ����ȷ�������񳤶ȡ� �ļ���������");
}

void MeshLoader_OGRE::ReadVertexBuffer(IFile* file, ChunkData& parent, OgreGeometry* geometry)
{
	OgreVertexBuffer* buffer = new OgreVertexBuffer();

	ReadShort(file, parent, buffer->BindIndex);
	ReadShort(file, parent, buffer->VertexSize);
	buffer->VertexSize /= sizeof(float32);

	Debug::LogInfo("Read Vertex Buffer VertexSize" + Convert((int32)buffer->VertexSize));

	ChunkData data;
	ReadChunkData(file, data);

	if (data.Header.ID == OGRE_GEOMETRY_VERTEX_BUFFER_DATA)
	{
		int32 count = buffer->VertexSize * geometry->VertexCount;
		buffer->Data = new float32[count];

		for (int32 i = 0;i < count;i++)
		{
			float32 num;
			ReadFloat(file, data, num);
			buffer->Data[i] = num;
		}
	}

	geometry->Buffers.push_back(buffer);
	parent.Read += data.Read;

	if (parent.Read != parent.Header.Length)
		Debug::LogError("ReadVertexBuffer ����ȷ�������񳤶ȡ� �ļ���������");
}

#pragma region ��ȡ Skeleton

// ���ݽṹ
// |Bone|,|Bone|,|...|,|BoneParent|,|BoneParent|,|...|
// |Animation|,|AnimationTrack|,|Keyframe|
// |Animation|,|AnimationTrack|,|Keyframe|,|...|
void MeshLoader_OGRE::LoadSkeleton(string& name)
{
	Debug::LogInfo("Load Skeleton is :" + name);

	string filePath = "../media/" + name;
	IFile* file = (FIleInfo*)FIleInfo::Open(filePath, READ_ONLY);

	if (!file)
	{
		Debug::LogWarning("�޷�����ƥ��Ĺ���:" + name);
		return;
	}

	int16 id;
	file->Read(&id, 2);
	if (swapEndian)
		id = bswap_16(id);

	if (id != OGRE_SKELETON_HEADER)
		return;

	string version;
	ChunkData head;
	ReadString(file, head, version);
	uint16 BoneID = 0;
	Animation* animation = 0;

	//static int32 count = 0;
	//static string preName;

	while (file->GetPos() < file->GetFileSize())
	{
		ChunkData data;
		ReadChunkData(file, data);

		//Debug::LogError("----------------------�ֽ���----------------------");

		switch (data.Header.ID)
		{
		case OGRE_SKELETON_BONE:                    // 0x2000, ����
		{
			Bone* bone = skeleton->AddBone();

			ReadString(file, data, bone->Name);
			ReadShort(file, data, bone->BoneID);
			ReadVector4D(file, data, bone->Position);
			ReadQuaternion(file, data, bone->Rotate);

			//Debug::LogInfo("-------------------------------------------------------");
			//Debug::LogInfo("��������=" + bone->Name);
			//Debug::LogInfo("Position:", bone->Position);
			//Debug::LogInfo("Rotate:",   bone->Rotate);

			uint32 lengthSize = data.Header.Length + bone->Name.size();

			if (data.Read < lengthSize)
			{
				ReadVector3D(file, data, bone->Scale);
			}
			else
			{
				bone->Scale = Vector3::one();
			}
			bone->Parent = 0xFFFF;

			if (bone->Name == "left_upper_wing")
			{
				Debug::LogInfo("Rotate:", bone->Rotate);
			}

			QuaternionToMatrix(bone->Rotate, bone->LocalBindMatrix);
			if (bone->Position != Vector4::zero())
			{
				bone->LocalBindMatrix[3] = bone->Position.x;
				bone->LocalBindMatrix[7] = bone->Position.y;
				bone->LocalBindMatrix[11] = bone->Position.z;
				bone->LocalBindMatrix[15] = 1;
			}
			if (bone->Scale != Vector3::one())
			{
				bone->LocalBindMatrix[0] *= bone->Scale.x;
				bone->LocalBindMatrix[1] *= bone->Scale.y;
				bone->LocalBindMatrix[2] *= bone->Scale.z;

				bone->LocalBindMatrix[4] *= bone->Scale.x;
				bone->LocalBindMatrix[5] *= bone->Scale.y;
				bone->LocalBindMatrix[6] *= bone->Scale.z;

				bone->LocalBindMatrix[8] *= bone->Scale.x;
				bone->LocalBindMatrix[9] *= bone->Scale.y;
				bone->LocalBindMatrix[10] *= bone->Scale.z;
			}

		}break;
		case OGRE_SKELETON_BONE_PARENT:              // 0x3000, �����ṹ
		{
			uint16 parent;

			ReadShort(file, data, BoneID);
			ReadShort(file, data, parent);

			vector<Bone*> bones = skeleton->GetAllBones();

			if (BoneID < bones.size() &&
				parent < bones.size())
			{
				bones[BoneID]->Parent = parent;
			}
		}break;
		case OGRE_SKELETON_ANIMATION:                // 0x4000, ����
		{
			string name;
			float32 length;
			ReadString(file, data, name);
			ReadFloat(file, data, length);

			animation = skeleton->AddAnimation(name);
			animation->Name = name;
			animation->Length = length;

			Debug::LogInfo("AanimationName:" + name + " Length:" + Convert(length));
		}break;
		case OGRE_SKELETON_ANIMATION_TRACK:  
		{
			ReadShort(file, data, BoneID);
		}break;
		// �ؼ�֡��λ�û�ȡ
		case OGRE_SKELETON_ANIMATION_TRACK_KEYFRAME:  // 0x4110,
		{
			float32 Time;
		    Quaternion Rotate = Quaternion::identity();
			Vector4 Position = Vector4::zero();
			Vector3 Scale = Vector3::zero();

			ReadFloat(file, data, Time);
			// ���λ��
			ReadQuaternion(file, data, Rotate);
			ReadVector4D(file, data, Position);

			vector<Bone*> bones = skeleton->GetAllBones();
			Bone* bone = bones[BoneID];

			if (data.Read < data.Header.Length)
			{
				ReadVector3D(file, data, Scale);
			}
			else
			{
				Scale = Vector3::one();
			}
			
			PositionKeyFrame* keyFrame = new PositionKeyFrame();
			keyFrame->BoneID = BoneID;
			keyFrame->Position = Position + bone->Position;
			keyFrame->Position.w = 1;
			keyFrame->Time = Time;
			animation->PostionKeyFrames[BoneID].push_back(keyFrame);

			RotateKeyFrame* PkeyFrame = new RotateKeyFrame();
			PkeyFrame->BoneID = BoneID;
			PkeyFrame->Rotate = bone->Rotate * Rotate;
			PkeyFrame->Time = Time;

			/*if (bone->Name == "lower_torso")*/
			{
				//count++;
				//if (bone->Name != preName)
				//{
				//	Debug::LogInfo("-------------------" + Convert(count) + "-------------------");
				//	count = 0;
				//}
				//Debug::LogInfo(Convert(count));
				//Debug::LogInfo("BONE_NAME=" + bone->Name);
				//Debug::LogInfo("Timer:" + Convert(Time));
				//Debug::LogInfo("������ת=:", bone->Rotate);
				//Debug::LogInfo("�ؼ�֡��ת=:", Rotate);
				//Debug::LogInfo("�������=:", PkeyFrame->Rotate);
			}

			animation->RotateKeyFrames[BoneID].push_back(PkeyFrame);

		/*	preName = bone->Name;*/

			if (Scale != Vector3::one())
			{
				ScaleKeyFrame* keyFrame = new ScaleKeyFrame();
				keyFrame->BoneID = BoneID;
				keyFrame->Scale = Scale;
				keyFrame->Time = Time;

				animation->ScaleKeyFrames[BoneID].push_back(keyFrame);
			}
		}break;
		default:
		{
			int32 length = data.Header.Length - data.Read;
			file->Seek(length, SEEK_CUR);
			data.Read += length;
		}break;
		}
	}

	int32 length = 1;
}

#pragma endregion

#pragma region ��ȡ����

// TODO??�����費Ҫ��Ҫ
void MeshLoader_OGRE::LoadMaterials(OgreSubMesh* subMesh)
{
	Debug::LogInfo("Load Materials:" + subMesh->MaterialName);

	string dirName = FIleInfo::GetFileDir(subMesh->MaterialName);

	string fileName = "../media/materials/" + dirName + ".material";

	FIleInfo* file = (FIleInfo*)FIleInfo::Open(fileName, READ_ONLY);

	if (!file)
	{
		Debug::LogWarning("�޷�����ƥ��Ĳ���:" + fileName);
		return;
	}

	string token;
	GetMaterialToken(file, token);

	int32 MaterialsNumber = 0;

	while (file->GetPos() < file->GetFileSize())
	{
		if (token == "/*")
		{
			while (token != "*/")
			{
				GetMaterialToken(file, token);
			}
			GetMaterialToken(file, token);
			continue;
		}

		if (token == "vertex_program" ||
			token == "fragment_program" ||
			token == "tessellation_hull_program" ||
			token == "tessellation_domain_program" ||
			token == "shared_params" ||
			token == "geometry_program" ||
			token == "tesselation_hull_program" ||
			token == "tesselation_domain_program")
		{
			// ����������
			uint32 blocks = 1;

			do
			{
				GetMaterialToken(file, token);
			} while (token != "{");

			do
			{
				GetMaterialToken(file, token);
				if (token == "{")
					++blocks;
				else if (token == "}")
					--blocks;
			} while (blocks);

			GetMaterialToken(file, token);
			continue;
		}

		if (token != "material")
		{
			Debug::LogError("���ʼ��ش���!");
			return;
		}

		string materialName;
		GetMaterialToken(file, materialName);
		Debug::LogInfo("��������:" + materialName);

		Debug::LogInfo("-----------------------------------Material_" + Convert(MaterialsNumber) + "(" + materialName + ")--------------------------------");

		GetMaterialToken(file, token);   // open brace
		GetMaterialToken(file, token);

		bool ReceiveShadows = false;
		bool TransparencyCastsShadows = false;

		while (token != "}")
		{
			if (token == "lod_values")
			{
				GetMaterialToken(file, token);
			}
			else if (token == "receive_shadows")
			{
				GetMaterialToken(file, token);
				ReceiveShadows = (token == "on");
			}
			else if (token == "transparency_casts_shadows")
			{
				GetMaterialToken(file, token);
				TransparencyCastsShadows = (token == "on");
			}
			else if (token == "technique")
			{
				ReadTechnique(file, subMesh, materialName);
			}
			GetMaterialToken(file, token);
		}

		GetMaterialToken(file, token);

		MaterialsNumber++;
		Debug::LogInfo("---------------------------------MaterialEnd----------------------------------");
	}
}

void MeshLoader_OGRE::ReadTechnique(
	IFile* file, OgreSubMesh* subMesh, const string& materialName)
{
	string token;
	string techniqueName;
	GetMaterialToken(file, techniqueName);

	if (techniqueName == "{")
	{
		techniqueName = "û������!";
	}
	else
	{
		GetMaterialToken(file, token);  // open brace
	}

	Debug::LogInfo("-----------------------------Technique(" + techniqueName + ")---------------------------");

	GetMaterialToken(file, token);

	while (token != "}")
	{
		if (token == "pass")
		{
			ReadPass(file,subMesh, materialName);
		}
		else if (token == "lod_index")
			GetMaterialToken(file, token);
		else if (token == "scheme")
			GetMaterialToken(file, token);

		GetMaterialToken(file, token);
	}

	Debug::LogInfo("-------------------------------TechniqueEnd-------------------------");
}

void MeshLoader_OGRE::ReadPass(
	IFile* file, OgreSubMesh* subMesh, const string& materialName)
{
	string token;
	string passName;

	GetMaterialToken(file, passName);
	if (passName == "{")
	{
		passName = "�]������!";
	}
	else
	{
		GetMaterialToken(file, token);  // open brace
	}

	Debug::LogInfo("----------------------PassName(" + passName + ")-----------------------");

	GetMaterialToken(file, token);
	if (token == "}")
		return;

	uint32 inBlocks = 1;
	uint32 textureUnit = 0;

	ColorF ambientColor;
	ColorF diffuseColor;
	ColorF specularColor;
	ColorF emissiveColor;

	while (inBlocks)
	{
		if (token == "ambient")
		{
			ReadColor(file, ambientColor);
			Debug::LogInfo("ambient:" +
				Convert(ambientColor.GetRed()) + "," +
				Convert(ambientColor.GetGreen()) + "," +
				Convert(ambientColor.GetBlue()) + "," +
				Convert(ambientColor.GetAlpha()));
		}
		else if (token == "diffuse")
		{
			ReadColor(file, diffuseColor);

			Debug::LogInfo("diffuse:" +
				Convert(diffuseColor.GetRed()) + "," +
				Convert(diffuseColor.GetGreen()) + "," +
				Convert(diffuseColor.GetBlue()) + "," +
				Convert(diffuseColor.GetAlpha()));
		}
		else if (token == "specular")
		{
			ReadColor(file, specularColor);

			Debug::LogInfo("specular:" +
				Convert(specularColor.GetRed()) + "," +
				Convert(specularColor.GetGreen()) + "," +
				Convert(specularColor.GetBlue()) + "," +
				Convert(specularColor.GetAlpha()));
		}
		else if (token == "emissive")
		{
			ReadColor(file, emissiveColor);

			Debug::LogInfo("emissive:" +
				Convert(emissiveColor.GetRed()) + "," +
				Convert(emissiveColor.GetGreen()) + "," +
				Convert(emissiveColor.GetBlue()) + "," +
				Convert(emissiveColor.GetAlpha()));
		}
		else if (token == "scene_blend")
		{
			GetMaterialToken(file, token);

			if (token == "add")
			{
				Debug::LogInfo("���ģʽ:add");
			}
			else if (token == "modulate")
			{
				Debug::LogInfo("���ģʽ:modulate");
			}
			else if (token == "alpha_blend")
			{
				Debug::LogInfo("���ģʽ:alpha_blend");
			}
			else if (token == "colour_blend")
			{
				Debug::LogInfo("���ģʽ:colour_blend");
			}
			else
				GetMaterialToken(file, token);
		}
		else if (token == "depth_check")
		{
			GetMaterialToken(file, token);
			Debug::LogInfo("ZBuffer:" + token);
		}
		else if (token == "depth_write")
		{
			GetMaterialToken(file, token);
			Debug::LogInfo("���д��:" + token);
		}
		else if (token == "depth_func")
		{
			// always_fail
			// always_pass
			// equal
			// greater
			// greater_equal
			// less
			// less_equal
			// not_equal
			GetMaterialToken(file, token);
			Debug::LogInfo("��ȱȽϺ���:" + token);
		}
		else if (token == "normalise_normals")
		{
			GetMaterialToken(file, token);
			Debug::LogInfo("�Ƿ�������:" + token);
		}
		else if (token == "depth_bias")
		{
			GetMaterialToken(file, token); // bias value
		}
		else if (token == "alpha_rejection")
		{
			GetMaterialToken(file, token); // function name
			GetMaterialToken(file, token); // value

			Debug::LogInfo("alpha_rejection:" + token);
		}
		else if (token == "alpha_to_coverage")
		{
		GetMaterialToken(file, token);
			Debug::LogInfo("�Ƿ���������:" + token);
		}
		else if (token == "colour_write")
		{
		GetMaterialToken(file, token);
			Debug::LogInfo("ColorMask:" + token);
		}
		else if (token == "cull_hardware")
		{
		GetMaterialToken(file, token);
		}
		else if (token == "cull_software")
		{
		GetMaterialToken(file, token);
		}
		else if (token == "lighting")
		{
		GetMaterialToken(file, token);
			Debug::LogInfo("��������:" + token);
		}
		else if (token == "shading")
		{
		GetMaterialToken(file, token);
			Debug::LogInfo("GouraudShading:" + token);
		}
		else if (token == "polygon_mode")
		{
			// wireframe
			// points
			GetMaterialToken(file, token);
			Debug::LogInfo("ͼ��д��ģʽ:" + token);
		}
		else if (token == "max_lights")
		{
		GetMaterialToken(file, token);
			Debug::LogInfo("����������:" + token);
		}
		else if (token == "point_size")
		{
		GetMaterialToken(file, token);
			Debug::LogInfo("��Ĵ�С:" + token);
		}
		else if (token == "point_sprites")
		{
		GetMaterialToken(file, token);
			Debug::LogInfo("point_sprites:" + token);
		}
		else if (token == "point_size_min")
		{
		    GetMaterialToken(file, token);
			Debug::LogInfo("point_size_min:" + token);
		}
		else if (token == "point_size_max")
		{
		    GetMaterialToken(file, token);
			Debug::LogInfo("point_size_max:" + token);
		}
		else if (token == "texture_unit")
		{
		    GetMaterialToken(file, token);
			Debug::LogInfo("��������:" + token);
			GetMaterialToken(file, token); // open brace

			while (token != "}")
			{
				if (token == "texture")
				{
					GetMaterialToken(file, token);
					
					if (materialName == subMesh->MaterialName)
						subMesh->TextureName = token;

					Debug::LogInfo("Texture Name:" + token);
				}
				else if (token == "tex_address_mode")
				{
					GetMaterialToken(file, token);
					Debug::LogInfo("tex_address_mode:" + token);
				}
				else if (token == "filtering")
				{
					GetMaterialToken(file, token);
					// point
					// linear
					// ������
					// �������Թ���
					Debug::LogInfo("�������:" + token);

				}
				else if (token == "max_anisotropy")
				{
					GetMaterialToken(file, token);
					Debug::LogInfo("�������Թ��˵ȼ�:" + token);
				}
				else if (token == "texture_alias")
				{
					GetMaterialToken(file, token);
				}
				else if (token == "mipmap_bias")
				{
					GetMaterialToken(file, token);
					Debug::LogInfo("MipMap Level:" + token);
				}
				else if (token == "colour_op")
				{
					GetMaterialToken(file, token);
					Debug::LogInfo("��ɫ���:" + token);
					if (token == "add")
					{

					}
					else if (token == "modulate")
					{

					}
					else if (token == "alpha_blend")
					{

					}
					else if (token == "colour_blend")
					{

					}
					else
						GetMaterialToken(file, token);
				}

				GetMaterialToken(file, token);
			}
			textureUnit++;
		}
		else if (token == "shadow_caster_program_ref")
		{
			while (token != "}")
			{
				GetMaterialToken(file, token);
			}
		}
		else if (token == "shadow_caster_vertex_program_ref")
		{
			while (token != "}")
			{
				GetMaterialToken(file, token);
			}
		}
		else if (token == "vertex_program_ref")
		{
			while (token != "}")
			{
				GetMaterialToken(file, token);
			}
		}

		GetMaterialToken(file, token);
		if (token == "{")
			inBlocks++;
		else if (token == "}")
			inBlocks--;

	}

	Debug::LogInfo("----------------------PassEnd------------------------");
}

void MeshLoader_OGRE::ReadColor(IFile* file, ColorF& color)
{
	ReadFloat32(file, color.GetRed());
	ReadFloat32(file, color.GetGreen());
	ReadFloat32(file, color.GetBlue());
	ReadFloat32(file, color.GetAlpha());
}

void MeshLoader_OGRE::ReadFloat32(IFile* file, float32& value)
{
	int8 c;
	file->Read(&c, sizeof(int8));
	string numberStr;
	bool haveNumber = false;

	while (Isspace(c))
	{
		file->Read(&c, sizeof(int8));
	}

	if (!IsNumber(c))
	{
		file->Seek(-1, SEEK_CUR);
	}
	else
	{
		while (IsNumber(c))
		{
			haveNumber = true;
			numberStr += c;
			file->Read(&c, sizeof(int8));
		}
		if (haveNumber && numberStr != "")
			value = float32(atof(numberStr.c_str()));
		else
			value = 1;
	}
}

#pragma endregion


//---------------------------------Read ��Ϣ------------------------//

void MeshLoader_OGRE::GetMaterialToken(IFile* file, string& token)
{
	bool parseString = false;
	int8 c = 0;
	token = "";

	if (file->GetPos() >= file->GetFileSize())
	{
		token = "}";
		return;
	}

	file->Read(&c, sizeof(int8));

	bool readSpace = false;
	while (isspace(c) && file->GetPos() < file->GetFileSize())
	{
		readSpace = true;
		file->Read(&c, sizeof(int8));
	}

	while (file->GetPos() < file->GetFileSize() && !isspace(c))
	{
		if (c == '/')  //�����ע��
		{
			file->Read(&c, sizeof(int8));
			if (c == '/')
			{
				while (c != '\n')
				{
					file->Read(&c, sizeof(int8));
				}
				while (c == '\n')
				{
					file->Read(&c, sizeof(int8));
				}
				continue;
			}
			else if (c == '*')
			{
				token += '/';
				token += '*';
				break;
			}
			else
				token += '/';
		}
		else if (c == '*')
		{
			file->Read(&c, sizeof(int8));
			if (c == '/')
			{
				token = "";
				token += '*';
				token += '/';
				break;
			}
		}

		token += c;
		file->Read(&c, sizeof(int8));
	}
}


void MeshLoader_OGRE::ReadChunkData(IFile* file,ChunkData& data)
{
	file->Read(&data.Header, sizeof(ChunkHeader));
	if (swapEndian)
	{
		data.Header.ID = bswap_16(data.Header.ID);
		data.Header.Length = bswap_32(data.Header.Length);
	}

	data.Read += sizeof(ChunkHeader);
}

void MeshLoader_OGRE::ReadString(IFile* file, ChunkData& data, string& out)
{
	int8 c = 0;

	while (c != '\n')
	{
		file->Read(&c, sizeof(int8));
		if (c != '\n')
		{
			out += c;
		}
	}
	data.Read += out.size() + 1;
}

void MeshLoader_OGRE::ReadBool(IFile* file, ChunkData& data, bool& out)
{
	int8 c = 0;
	file->Read(&c, sizeof(int8));
	out = c != 0;
	++data.Read;
}

void MeshLoader_OGRE::ReadInt32(IFile* file, ChunkData& data, int32& out)
{
	file->Read(&out, sizeof(int32));
	if (swapEndian)
	{
		out = bswap_32(out);
	}
	data.Read += sizeof(int32);
}

void MeshLoader_OGRE::ReadShort(IFile* file, ChunkData& data, uint16& out)
{
	file->Read(&out, sizeof(uint16));
	if (swapEndian)
	{
		out = bswap_16(out);
	}
	data.Read += sizeof(uint16);
}


void MeshLoader_OGRE::ReadFloat(IFile* file, ChunkData& data, float32& out)
{
	file->Read(&out, sizeof(float32));
	if (swapEndian)
	{
		out = (float32)(bswap_32((unsigned long)out));
	}
	data.Read += sizeof(float32);
}

void MeshLoader_OGRE::ReadVector3D(IFile* file, ChunkData& data, Vector3& out)
{
	ReadFloat(file, data, out.x);
	ReadFloat(file, data, out.y);
	ReadFloat(file, data, out.z);
	out.z *= -1.0F;
}

void MeshLoader_OGRE::ReadVector4D(IFile* file, ChunkData& data, Vector4& out)
{
	ReadFloat(file, data, out.x);
	ReadFloat(file, data, out.y);
	ReadFloat(file, data, out.z);
	out.w = 1;
	out.z *= -1.0F;
}

void MeshLoader_OGRE::ReadQuaternion(IFile* file, ChunkData& data, Quaternion& out)
{
	ReadFloat(file, data, out.x);
	ReadFloat(file, data, out.y);
	ReadFloat(file, data, out.z);
	ReadFloat(file, data, out.w);
	out.z *= -1.0F;
}

bool MeshLoader_OGRE::IsNumber(char c)
{
	if (c == '.' || c == '-' || c == '0' ||
		c == '1' || c == '2' || c == '3' ||
		c == '4' || c == '5' || c == '6' ||
		c == '7' || c == '8' || c == '9')
	{
		return true;
	}

	return false;
}

bool MeshLoader_OGRE::Isspace(char c)
{
	if (c == ' ' ||
		c == '\f' ||
		c == '\n' ||
		c == '\r' ||
		c == '\t' ||
		c == '\v')
	{
		return true;
	}
	return false;
}