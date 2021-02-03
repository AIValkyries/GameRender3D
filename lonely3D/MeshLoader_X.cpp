#include "MeshLoader_X.h"
#include "UtilTools.h"
#include "Debug.h"
#include "IResourceItem.h"
#include "GameHost.h"

void MeshLoader_X::SetGameHost(GameHost* _gameHost)
{
	gameHost = _gameHost;
}

bool MeshLoader_X::IsALoadableFileExtension(const string& fileName)
{
	return IsFileExtension(fileName, "x");
}

bool MeshLoader_X::Loader(IFile* file, IResourceItem* item)
{
	skeleton = (SkinnedSkeleton*)item;

	if (!ReadFileIntoMemory(file))
	{
		Debug::LogError("文件读取失败!!");
		return false;
	}

	if (!ParseFile())
	{
		Debug::LogError("文件解析失败!!!");
		return false;
	}

	vector<lMaterial*> materials = mesh->Materials;

	for (uint32 i = 0;i < materials.size();i++)
	{
		MeshBuffer* meshBuffer = skeleton->AddMeshBuffer();
		meshBuffer->SetMaterial(materials[i]);
	}

	vector<MeshBuffer*> Buffers = skeleton->GetSkinMeshBuffer();

	// 顶点颜色=材质颜色
	for (uint32 i = 0;i < mesh->FaceMaterialIndices.size();i++)
	{
		for (uint32 id = i * 3 + 0; id < i * 3 + 2; id++)
		{
			uint32 vertexIndex = mesh->Indices->GetValue(id);
			ColorF color = Buffers[mesh->FaceMaterialIndices[i]]->GetMaterial()->DiffuseColor;
			mesh->Vertexs[vertexIndex]->color = color;
		}
	}

	// 顶点分组根据材质面索引
	vector<uint32> verticesLinkBuffer;
	for (uint32 i = 0; i < mesh->Vertexs.size(); i++)
	{
		verticesLinkBuffer.push_back(-1);
	}

	for (uint32 i = 0; i < mesh->FaceMaterialIndices.size(); i++)
	{
		for (uint32 id = i * 3 + 0; id <= i * 3 + 2; id++)
		{
			uint32 vertexIndex = mesh->Indices->GetValue(id);
			verticesLinkBuffer[vertexIndex] = mesh->FaceMaterialIndices[i];
		}
	}

	// 重新构建顶点缓存，和索引缓存
	uint32* vCountArray = new uint32[materials.size()];
	memset(vCountArray, 0, materials.size() * sizeof(uint32));

	for (uint32 i = 0; i < mesh->Vertexs.size(); i++)
	{
		int32 index = verticesLinkBuffer[i];
		if (index != -1)
			++vCountArray[index];
	}

	for (uint32 i = 0; i < materials.size(); i++)
	{
		MeshBuffer* buffer = Buffers[i];
		buffer->CreateVertex(vCountArray[i]);
	}

	// 计算顶点的新索引
	vector<uint32> verticesLinkIndex;

	uint32* indexs = new uint32[materials.size()];
	memset(indexs, 0, materials.size() * sizeof(uint32));

	for (uint32 i = 0; i < mesh->Vertexs.size(); i++)
	{
		if (verticesLinkBuffer[i] == -1)
			continue;
		int32 bufferID = verticesLinkBuffer[i];
		if (bufferID != 0 && bufferID != 1)
		{
			Debug::LogError("BufferID出错了!" + Convert(bufferID));
			break;
		}

		MeshBuffer* buffer = Buffers[bufferID];
		verticesLinkIndex.push_back(indexs[bufferID]++);

		buffer->SetSourceVertex(verticesLinkIndex[i], *mesh->Vertexs[i]);
		buffer->SetTransformVertex(verticesLinkIndex[i], *mesh->Vertexs[i]);
	}

	vector<uint16>* bufferIndexs = new vector<uint16>[materials.size()];
	for (uint32 i = 0; i < mesh->FaceMaterialIndices.size(); i++)
	{
		for (uint32 id = i * 3 + 0; id <= i * 3 + 2; id++)
		{
			int32 vertexIndex = verticesLinkIndex[mesh->Indices->GetValue(id)];
			int32 bufferID = mesh->FaceMaterialIndices[i];
			bufferIndexs[bufferID].push_back(uint16(vertexIndex));
		}
	}

	for (uint32 i = 0; i < materials.size(); i++)
	{
		MeshBuffer* buffer = Buffers[i];
		int32 indexSize = bufferIndexs[i].size();
		buffer->CreateIndices(indexType, indexSize);
	}

	memset(indexs, 0, materials.size() * sizeof(uint32));

	for (uint32 i = 0; i < materials.size(); i++)
	{
		for (uint32 j = 0; j < bufferIndexs[i].size(); j++)
		{
			MeshBuffer* buffer = Buffers[i];
			uint32 indexValue = bufferIndexs[i][j];
			uint32 index = indexs[i]++;
			buffer->SetIndices(index, indexValue);
		}
	}

	Debug::LogInfo("-----------------------------------------------");

	uint64 vertexTotalSize = 0;
	uint64 indicesTotalSize = 0;

	for (uint32 i = 0; i < Buffers.size(); i++)
	{
		MeshBuffer* buffer = Buffers[i];

		uint64 vertexSize  = buffer->GetVertexCount();
		uint64 indicesSize = buffer->GetIndexBuffer()->Size();

		vertexTotalSize += vertexSize;
		indicesTotalSize += indicesSize;

		Debug::LogInfo("顶点长度:" + Convert(vertexSize));
		Debug::LogInfo("索引长度:" + Convert(indicesSize));
	}

	Debug::LogInfo("-----------------------------------------------");
	Debug::LogInfo("总顶点长度:" + Convert(vertexTotalSize));
	Debug::LogInfo("总索引长度:" + Convert(indicesTotalSize));

	for (uint32 i = 0;i < mesh->TextureNames.size();i++)
	{
		const string textureFilePath = "../media/texture/" + mesh->TextureNames[i];
		Texture* texture = gameHost->GetTextureManager()->LoadResource(textureFilePath);
		Buffers[i]->SetTexture(texture);
	}

	vector<Bone*> boneAll = skeleton->GetAllBones();

	for (uint32 i = 0;i < boneAll.size();i++)
	{
		vector<BoneAssignment*> assignments = boneAll[i]->Weights;

		int32 aSize = assignments.size();

		if (aSize)
		{
			for (int32 j = 0;j < aSize;j++)
			{
				BoneAssignment* b = assignments[j];

				int32 vertexID = b->VertexID;

				b->BufferID = verticesLinkBuffer[vertexID];
				b->VertexID = verticesLinkIndex[vertexID];

				MeshBuffer* buffer = Buffers[b->BufferID];

				b->StaticPos = buffer->GetSourceVertex(b->VertexID)->position;
				b->StaticNormal = buffer->GetSourceVertex(b->VertexID)->normal;
			}
		}
	}

	delete vCountArray;
	delete indexs;
	delete mesh;
	delete buffer;
	buffer = 0;
	p = 0;
	end = 0;

	skeleton->Finalize();

	// TODO加载响应的材质纹理
	Debug::LogInfo("MeshX加载完成!!!!");

	return true;
}

bool MeshLoader_X::ReadFileIntoMemory(IFile* file)
{
 	buffer = new char[file->GetFileSize()];

	if (!file->Read(buffer,file->GetFileSize()))
	{
		Debug::LogError("文件读取失败,FileName=" + file->GetFullFileName());
		return false;
	}

	// 是否使用二进制
	if (strncmp(&buffer[8], "txt ", 4) == 0)
	{
		binaryFormat = false;
	}
	else if (strncmp(&buffer[8], "bin ", 4) == 0)
	{
		binaryFormat = true;
	}

	if (strncmp(&buffer[12], "0032", 4) == 0)
	{
		floatSize = 4;
		indexType = INDEX_16BIT;
	}
	else if (strncmp(&buffer[12], "0064", 4) == 0)
	{
		floatSize = 8;
		indexType = INDEX_32BIT;
	}

	end = buffer + file->GetFileSize();
	p = &buffer[16];

	return true;
}

bool MeshLoader_X::ParseFile()
{
	while (true)
	{
		string objectName = GetToken(); 
		if (objectName.size() == 0 || objectName == "}")
			break;

		if (objectName == "template")
		{
			if (!ReadDataObjectEnd())
				return false;
		}
		else if (objectName == "Frame")
		{
			if (!ReadFrame(0))
				return false;
		}
		else if (objectName == "AnimationSet")
		{
			if (!ReadAnimationSet())
				return false;
		}
	}
	return true;
}

bool MeshLoader_X::ReadFrame(Bone* parent)
{
	string jointName = GetToken();
	ReadLeftBrace();

	Debug::LogInfo("JointName=" + jointName);

	if (jointName.size() == 0)
	{
		Debug::LogError("文件读取出错，JointName为空!!!");
		return false;
	}
	 
	if (!mesh) mesh = new XMesh();
	Bone* joint = 0;
	uint32 boneID;

	vector<Bone*> boneAll = skeleton->GetAllBones();

	for (uint32 i = 0;i < boneAll.size();i++)
	{
		if (boneAll[i]->Name == jointName)
		{
			joint = boneAll[i];
			boneID = i;
			break;
		}
	}

	if (joint == 0)
	{
		joint = skeleton->AddBone();
		if (parent)
			parent->Children.push_back(joint);
		boneID = skeleton->GetBoneSize() - 1;
	}

	joint->Name = jointName;
	joint->BoneID = uint16(boneID);
	joint->Parent = 0xFFFF;
	if (parent)
		joint->Parent = parent->BoneID;

	while (true)
	{
		string token = GetToken();
		if (token.size() == 0 || token == "}")
			break;

		if (token == "FrameTransformMatrix")
		{
			if (!ReadTransformationMatrix(joint->LocalBindMatrix))
				return false;
		}
		else if (token == "Frame")
		{
			if (!ReadFrame(joint))
				return false;
		}
		else if (token == "Mesh")
		{
			if (!ReadMesh(mesh))
				return false;
		}
	}
	
	return true;
}

bool MeshLoader_X::ReadTransformationMatrix(Matrix4x4& mat)
{
	if (!ReadLeftBrace())
	{
		Debug::LogError("在x档案中找不到Transformation Matrix中的左括号");
		return false;
	}

	mat = ReadMatrix4x4();

	if (!ReadClosingBrace())
	{
		Debug::LogError("在x文件中的Transformation Matrix中找不到右括号");
		return false;
	}

	return true;
}

bool MeshLoader_X::ReadMesh(XMesh* mesh)
{
	Debug::LogInfo("\n\n开始读取网格---------------------------------");

	string name = GetToken();
	if (name.size() == 0)
	{
		Debug::LogError("网格名字为空!!!");
		return false;
	}

	if (!ReadLeftBrace())
	{
		Debug::LogError("左括号为空!!!");
		return false;
	}

	const uint32 nVertices = ReadInt32();

	ReadSemicolon();

	for (uint32 i = 0;i < nVertices;i++)
	{
		Vertex* vertex = new Vertex();
		vertex->position = ReadVector4();
		mesh->Vertexs.push_back(vertex);
	}

	Debug::LogInfo("网格顶点数量:" + Convert(nVertices));
	
	if (!ReadSemicolon())
	{
		Debug::LogError("顶点不存在分号结束!!!!");
		return false;
	}

	const uint32 nFaces = ReadInt32();
	IndexBuffer* indexBuffer = new IndexBuffer(indexType);

	ReadSemicolon();

	for (uint32 i = 0;i < nFaces;i++)
	{
		uint32 fcnt = ReadInt32();
		ReadSemicolon();

		if (fcnt == 3)
		{
			uint32 index0 = ReadInt32();
			uint32 index1 = ReadInt32();
			uint32 index2 = ReadInt32();

			indexBuffer->PushBack(index2);
			indexBuffer->PushBack(index1);
			indexBuffer->PushBack(index0);

			ReadSemicolon();
			ReadComma();
		}
	}

	mesh->Indices = indexBuffer;
	Debug::LogInfo("网格面数量:" + Convert(nFaces));

	if (!ReadSemicolon())
	{
		Debug::LogError("在x文件中找不到网格顶点阵列中的精加工分号");
		return false;
	}

	ReadSemicolon();

	while (true)
	{
		string objectName = GetToken();

		if (objectName.size() == 0)
		{
			Debug::LogError("在x文件的Mesh中发现意外的结尾");
			return false;
		}
		else if (objectName == "}")
			break;
		else if (objectName == "MeshNormals")
		{
			if (!ReadMeshNormals(mesh))
				return false;
		}
		else if (objectName == "MeshTextureCoords")
		{
			if (!ReadMeshTextureCoords(mesh))
				return false;
		}
		else if (objectName == "VertexDuplicationIndices")
		{
			if (!ReadDataObjectEnd())
				return false;
		}
		else if (objectName == "MeshMaterialList")
		{
			if (!ReadMeshMaterialList(mesh))
				return false;
		}
		else if (objectName == "XSkinMeshHeader")
		{
			if (!ReadXSkinMeshHeader(mesh))
				return false;
		}
		else if (objectName == "SkinWeights")
		{
			if (!ReadSkinWeights(mesh))
				return false;
		}
	}

	return true;
}

bool MeshLoader_X::ReadMeshNormals(XMesh* mesh)
{
	ReadLeftBrace();

	const uint32 nNormals = ReadInt32();

	ReadSemicolon();

	Vector3* normals = new Vector3[nNormals];

	for (uint32 i = 0;i < nNormals;i++)
	{
		normals[i] = ReadVector3();
	}

	Debug::LogInfo("法线数量:" + Convert(nNormals));

	if (!ReadSemicolon())
	{
		Debug::LogError("在x文件中找不到网格法线阵列中的精加工分号");
		return false;
	}

	const uint32 nFNormals = ReadInt32();
	ReadSemicolon();

	uint32  normalidx = 0;
	uint32* polygonfaces = new uint32[3];

	for (uint32 i = 0; i < nFNormals; i++)
	{
		const uint32 fct = ReadInt32();
		ReadSemicolon();

		uint32 vindex0 = mesh->Indices->GetValue(normalidx++);
		uint32 vindex1 = mesh->Indices->GetValue(normalidx++);
		uint32 vindex2 = mesh->Indices->GetValue(normalidx++);

		for (uint32 h = 0; h < fct; h++)
		{
			polygonfaces[h] = ReadInt32();
			ReadSemicolon();
			ReadComma();
		}

		uint32 nindex0 = polygonfaces[0];
		uint32 nindex1 = polygonfaces[1];
		uint32 nindex2 = polygonfaces[2];

		mesh->Vertexs[vindex0]->normal = normals[nindex0];
		mesh->Vertexs[vindex1]->normal = normals[nindex1];
		mesh->Vertexs[vindex2]->normal = normals[nindex2];
	}

	Debug::LogInfo("法线面数量:" + Convert(nFNormals));

	if (!ReadSemicolon())
	{
		Debug::LogError("在x文件中找不到网格面法线阵列中的精加工分号");
		return false;
	}
	ReadSemicolon();

	if (!ReadClosingBrace())
	{
		Debug::LogError("在x文件中找不到网格法线中的右括号在" + Convert((uint32)383));
		return false;
	}

	return true;
}

bool MeshLoader_X::ReadMeshTextureCoords(XMesh* mesh)
{
	ReadLeftBrace();

	const uint32 nCoords = ReadInt32();

	ReadSemicolon();

	for (uint32 i = 0; i < nCoords; i++)
	{
		mesh->Vertexs[i]->uv = ReadVector2();
	}

	Debug::LogInfo("UV纹理坐标数量:" + Convert(nCoords));

	if (!ReadSemicolon())
	{
		Debug::LogError("在x文件中找不到网格纹理坐标阵列中的精加工分号");
		return false;
	}
	ReadSemicolon();

	if (!ReadClosingBrace())
	{
		Debug::LogError("在x文件中找不到网格纹理坐标数组中的右括号");
		return false;
	}

	return true;
}

bool MeshLoader_X::ReadMeshMaterialList(XMesh* mesh)
{
	ReadLeftBrace();

	const uint32 materialCount = ReadInt32();
	ReadSemicolon();

	const uint32 nFaceIndices = ReadInt32();
	ReadSemicolon();


	Debug::LogInfo("材质数量:" + Convert(materialCount));

	for (uint32 i = 0;i < nFaceIndices;i++)
	{
		mesh->FaceMaterialIndices.push_back(ReadInt32());
	}

	if (!binaryFormat)
	{
		if (p[0] == ';')
		{
			p++;
		}
	}

	while (true)
	{
		string objectName = GetToken();
		if (objectName.size() == 0)
		{
			Debug::LogError(".x文件中的网格材质列表中发现意外的结尾");
			return false;
		}
		else if (objectName == "}")
			break;
		else if (objectName == "Material")
		{
			if (!ReadMaterial(mesh))
				return false;
		}
	}

	return true;
}

bool MeshLoader_X::ReadMaterial(XMesh* mesh)
{
	string materialName = GetToken();
	Debug::LogInfo("材质名称=" + materialName);

	if (materialName.size() == 0)
	{
		Debug::LogError("没找到材质的名字");
		return false;
	}

	if (!ReadLeftBrace())
	{
		Debug::LogError("没有开始的{符号!!!!");
		return false;
	}

	lMaterial* material = new lMaterial();
	mesh->Materials.push_back(material);

	material->DiffuseColor = ReadRGBA();
	ReadSemicolon();

	material->Power = ReadFloat();

	material->SpecularColor = ReadRGB();
	ReadSemicolon();

	material->EmissiveColor = ReadRGB();
	ReadSemicolon();

	// 读取名字
	while (true)
	{
		string objectName = GetToken();
		if (objectName.size() == 0)
		{
			Debug::LogError("x文件中的网格材质中发现意外的结尾");
			return false;
		}
		else if (objectName == "}")
			break;
		else if (objectName == "TextureFileName")
		{
			string textureFileName;
			while (p < end)
			{
				while ((p < end) && isspace(p[0]))
				{
					p++;
				}

				if (p[0] == '{')
				{
					p++;
				}
				else if (p[0] == '"')
				{
					p++;
				}
				else if (p[0] == ';')
				{
					p++;
					break;
				}
				else
				{
					textureFileName += p[0];
					p++;
				}
			}

			GetToken();

			mesh->TextureNames.push_back(textureFileName);
			Debug::LogInfo("材质的纹理:" + textureFileName);
		}
	}

	return true;
}

bool MeshLoader_X::ReadXSkinMeshHeader(XMesh* mesh)
{
	ReadLeftBrace();
	ReadInt32();
	ReadSemicolon();

	ReadInt32();
	ReadSemicolon();

	ReadInt32();
	ReadSemicolon();    //  读取分号

	ReadClosingBrace(); // 读取 "}"
	return true;
}

bool MeshLoader_X::ReadSkinWeights(XMesh* mesh)
{
	ReadLeftBrace();

	string jointName = GetNextTokenAsString();

	ReadSemicolon();

	if (jointName.size() == 0)
	{
		Debug::LogError("从.x文件中的Transfrom节点名称字符串读取时的语法未知");
		return false;
	}

	Debug::LogInfo("权重关节名称=" + jointName);

	//根据名称找到 Joint
	Bone* joint = 0;
	int32 jointID = 0;

	vector<Bone*> boneAll = skeleton->GetAllBones();

	for (uint32 i = 0;i < boneAll.size();i++)
	{
		if (boneAll[i]->Name == jointName)
		{
			joint = boneAll[i];
			jointID = i;
			break;
		}
	}

	const uint32 nWeights = ReadInt32();
	ReadSemicolon();
	
	for (uint32 i = 0;i < nWeights;i++)
	{
		BoneAssignment* assignment = new BoneAssignment();
		assignment->VertexID = ReadInt32();
		assignment->BoneID = uint16(jointID);
		
		joint->Weights.push_back(assignment);
	}

	Debug::LogInfo(jointName + ":关节绑定顶点数量:" + Convert(nWeights));
	ReadSemicolon();

	for (uint32 i = 0;i < nWeights;i++)
	{
		joint->Weights[i]->Weight = ReadFloat();
	}

	joint->GlobaBindInverseMatrix = ReadMatrix4x4();

	if (!ReadClosingBrace())
	{
		Debug::LogError("在x文件中找不到“皮肤权重”中的右括号");
		return false;
	}

	return true;
}

bool MeshLoader_X::ReadAnimationSet()
{
	Debug::LogInfo("\n\n读取动画!----------------------------------------------");

	string animationName = GetToken();
	if (animationName.size() == 0)
	{
		Debug::LogError("动画名字读取为空!!!!");
		return false;
	}

	Debug::LogInfo("动画名称:" + animationName);

	Animation* animation = skeleton->AddAnimation(animationName);

	ReadLeftBrace();

	while (true)
	{
		string objectName = GetToken();

		if (objectName.size() == 0)
		{
			Debug::LogError("在x文件中设置的动画中发现意外的结尾");
			return false;
		}
		else if (objectName == "}")
			break;
		else if (objectName == "Animation")
		{
			if (!ReadAnimation(animation))
				return false;
		}
	}

	return true;
}

bool MeshLoader_X::ReadAnimation(Animation* animation)
{
	string jointName = GetAnimationName();

	if (jointName.size() == 0)
	{
		Debug::LogError("动画骨骼名字为空!");
		return false;
	}

	Debug::LogInfo("动画对应的骨骼:" + jointName);

	vector<Bone*> bones = skeleton->GetAllBones();
	Bone* currentBone = 0;
	for (uint32 i = 0;i < bones.size();i++)
	{
		if (bones[i]->Name == jointName)
		{
			currentBone = bones[i];
			break;
		}
	}

	while (true)
	{
		string objectName = GetToken();
		if (objectName.size() == 0)
		{
			Debug::LogError("AnimationKey没找到");
			return false;
		}
		else if (objectName == "}")
			break;
		else if (objectName == "AnimationKey")
		{
			if (!ReadAnimationKey(animation, currentBone))
				return false;
		}
		else if (objectName == "AnimationOptions")
		{
			if (!ReadAnimationOptions())
				return false;
		}
	}

	return true;
}

bool MeshLoader_X::ReadAnimationKey(Animation* animation, Bone* bone)
{
	ReadLeftBrace();

	const uint32 kType = ReadInt32();

	if (kType > 4)
	{
		Debug::LogError("没有大于4的类型");
		return false;
	}

	ReadSemicolon();

	const uint32 frameKeys = ReadInt32();

	string info;

	if (kType == 0)
	{
		info = "rotation动画";
	}
	else if (kType == 1)
	{
		info = "scale动画";
	}
	else if (kType == 2)
	{
		info = "position动画";
	}

	Debug::LogInfo(info + "--帧数:" + Convert(frameKeys));

	ReadSemicolon();

	for (uint32 i = 0;i < frameKeys;i++)
	{
		const uint32 frame = ReadInt32();
		ReadSemicolon();

		switch (kType)
		{
		case 0:           // rotation
		{
			if (ReadInt32() != 4)
			{
				Debug::LogError("旋转关键帧使用四元数，应该有4个数字!");
				return false;
			}
			ReadSemicolon();

			float32 w = -ReadFloat();
			float32 x = -ReadFloat();
			float32 y = -ReadFloat();
			float32 z = -ReadFloat();

			if (!ReadSemicolon())
			{
				Debug::LogError("x文件中的四元数动画键后没有结束分号");
				return false;
			}

			RotateKeyFrame* PkeyFrame = new RotateKeyFrame();
			PkeyFrame->BoneID = bone->BoneID;
			PkeyFrame->Rotate.x = x;
			PkeyFrame->Rotate.y = y;
			PkeyFrame->Rotate.z = z;
			PkeyFrame->Rotate.w = w;

			PkeyFrame->Frame = float32(frame);

			animation->RotateKeyFrames[bone->BoneID].push_back(PkeyFrame);
		}break;          // scale
		case 1:
		{
			if (ReadInt32() != 3)
			{
				Debug::LogError("缩放关键帧是向量，应该要有3个数字");
				return false;
			}
			ReadSemicolon();

			float32 x = ReadFloat();
			float32 y = ReadFloat();
			float32 z = ReadFloat();


			ScaleKeyFrame* keyFrame = new ScaleKeyFrame();
			keyFrame->BoneID = bone->BoneID;
			keyFrame->Scale.x = x;
			keyFrame->Scale.y = y;
			keyFrame->Scale.z = z;

			keyFrame->Frame = float32(frame);

			animation->ScaleKeyFrames[bone->BoneID].push_back(keyFrame);

		}break;
		case 2:          // position
		{
			if (ReadInt32() != 3)
			{
				Debug::LogError("平移关键帧是向量，应该有3个数");
				return false;
			}
			ReadSemicolon();

			float32 x = ReadFloat();
			float32 y = ReadFloat();
			float32 z = ReadFloat();

			PositionKeyFrame* keyFrame = new PositionKeyFrame();
			keyFrame->BoneID = bone->BoneID;
			keyFrame->Position.x = x;
			keyFrame->Position.y = y;
			keyFrame->Position.z = z;
			keyFrame->Position.w = 1;
			keyFrame->Frame = float32(frame);
			animation->PostionKeyFrames[bone->BoneID].push_back(keyFrame);

		}break;
		}
		ReadSemicolon();
		ReadComma();

	}

	ReadSemicolon();
	ReadSemicolon();
	ReadSemicolon();
	ReadClosingBrace();

	return true;
}

bool MeshLoader_X::ReadAnimationOptions()
{
	ReadLeftBrace();
	ReadInt32();
	ReadSemicolon();
	ReadInt32();
	ReadSemicolon();
	ReadClosingBrace();
	return true;
}

//---------------------------------读取数据--------------------------------//

string MeshLoader_X::GetAnimationName()
{
	string outName;
	ReadDataObjectStart();
	FindNextNoneWhiteSpace();

	while (p < end)
	{
		if (p[0] == '{')
			p++;
		else if (p[0] == '}')
		{
			p++;
			break;
		}
		else
		{
			outName += p[0];
			p++;
		}
	}

	return outName;
}


// 读取分号
bool MeshLoader_X::ReadSemicolon()
{
	FindNextNoneWhiteSpace();

	if (p[0] == ';')
	{
		p++;
		return true;
	}

	return false;
}

// 读取右括号
bool MeshLoader_X::ReadClosingBrace()
{
	FindNextNoneWhiteSpace();

	if (p[0] == '}')
	{
		p++;
		return true;
	}

	return false;
}

bool MeshLoader_X::ReadComma()
{
	FindNextNoneWhiteSpace();
	if (p[0] == ',')
	{
		p++;
		return true;
	}
	return false;
}

// 读取左括号
bool MeshLoader_X::ReadLeftBrace()
{
	FindNextNoneWhiteSpace();

	if (p[0] == '{')
	{
		p++;
		return true;
	}

	return false;
}


string MeshLoader_X::GetToken()
{
	FindNextNoneWhiteSpace();

	string token;

	while ((p < end) && !Isspace(p[0]))
	{
		if (p[0] == '{' || p[0] == '}' ||
			p[0] == ';' || p[0] == ',' ||
			p[0] == ' ')
		{
			if (!token.size())
			{
				token += p[0];
				++p;
			}
			break;
		}
		token += p[0];
		p++;
	}

	return token;
}


string MeshLoader_X::GetNextTokenAsString()
{
	string outName;
	FindNextNoneWhiteSpace();

	if (p >= end)
		return 0;

	while (p < end)
	{
		if (p[0] == '{' || isspace(p[0]))
		{
			p++;
		}
		else
			break;
	}

	if (p[0] != '"')
		return 0;
	p++;

	while (p < end && p[0] != '"')
	{
		outName += p[0];
		++p;
	}
	if (p[1] != ';' || p[0] != '"')
		return "";
	p += 2;

	return outName;
}

// 忽略空格和注释
bool MeshLoader_X::FindNextNoneWhiteSpace()
{
	while (true)
	{
		// 如果是空格
		while (p < end && Isspace(p[0]))
		{
			p++;
		}
		if (p >= end)break;

		// 如果是注释
		while (p < end)
		{
			if (p[0] == '#' || (p[0] == '/' && p[1] == '/'))
			{
				ReadUntilEndOfLine();
			}
			else
				break;
		}

		if (!Isspace(p[0]))
			break; 	
	}
	return true;
}

//  读取到行结束
bool MeshLoader_X::ReadUntilEndOfLine()
{
	while (p < end)
	{
		if (p[0] == '\n' || p[0] == '\r')
		{
			p++;
			return true;
		}
		p++;
	}
	return false;
}

bool MeshLoader_X::ReadDataObjectStart()
{
	while (p < end)
	{
		if (p[0] == '{')
		{
			p++;
			return true;
		}
		p++;
	}
	return false;
}

// 读取至结束
bool MeshLoader_X::ReadDataObjectEnd()
{
	while (p < end)
	{
		if (p[0] == '}')
		{
			p++;
			return true;
		}
		p++;
	}
	return false;
}

bool MeshLoader_X ::Isspace(char c)
{
	if (c == ' ' ||
		c == '\f' ||
		c == '\n' ||
		c == '\r' ||
		c == '\t' ||
		c == '\v' ||
		c == '\\')
	{
		return true;
	}
	return false;
}

bool MeshLoader_X::IsFloat32(char c)
{
	if (c == '-' || c == '.' || c == '0' || c == '1' ||
		c == '2' || c == '3' || c == '4' || c == '5' ||
		c == '6' || c == '7' || c == '8' || c == '9')
	{
		return true;
	}
	return false;
}

bool MeshLoader_X::IsInt32(char c)
{
	if (c == '-' || c == '0' || c == '1' ||
		c == '2' || c == '3' || c == '4' || c == '5' ||
		c == '6' || c == '7' || c == '8' || c == '9')
	{
		return true;
	}
	return false;
}

float32 MeshLoader_X::ReadFloat()
{
	FindNextNoneWhiteSpace();

	string numberStr;

	while (p < end)
	{
		if (IsFloat32(p[0]))
		{
			numberStr += p[0];
			p++;
		}
		else
		{
			p++;
			break;
		}
	}

	return float32(atof(numberStr.c_str()));
}

int32 MeshLoader_X::ReadInt32()
{
	FindNextNoneWhiteSpace();

	string numberStr;

	while (p < end)
	{
		if (IsInt32(p[0]))
		{
			numberStr += p[0];
			p++;
		}
		else
		{
			ReadComma();
			break;
		}
	}
	 
	return int32(atof(numberStr.c_str()));
}


Vector2 MeshLoader_X::ReadVector2()
{
	Vector2 r;
	r.x = ReadFloat();
	r.y = ReadFloat();
	ReadComma();
	return r;
}

Vector3 MeshLoader_X::ReadVector3()
{
	Vector3 r;
	r.x = ReadFloat();
	r.y = ReadFloat();
	r.z = ReadFloat();
	ReadComma();
	return r;
}

Vector4 MeshLoader_X::ReadVector4()
{
	Vector4 r;
	r.x = ReadFloat();
	r.y = ReadFloat();
	r.z = ReadFloat();
	r.w = 1;
	ReadComma();
	return r;
}

Matrix4x4 MeshLoader_X::ReadMatrix4x4()
{
	Matrix4x4 mat;

	mat[0] = ReadFloat();
	mat[4] = ReadFloat();
	mat[8] = ReadFloat();
	mat[12] = ReadFloat();

	mat[1] = ReadFloat();
	mat[5] = ReadFloat();
	mat[9] = ReadFloat();
	mat[13] = ReadFloat();

	mat[2] = ReadFloat();
	mat[6] = ReadFloat();
	mat[10] = ReadFloat();
	mat[14] = ReadFloat();

	mat[3] = ReadFloat();
	mat[7] = ReadFloat();
	mat[11] = ReadFloat();
	mat[15] = ReadFloat();

	if (!ReadSemicolon())
		Debug::LogError("矩阵读取错误没有分号结束!!!");

	return mat;
}

ColorF MeshLoader_X::ReadRGBA()
{
	ColorF color;
	color.Set(ReadFloat(), ReadFloat(), ReadFloat(), ReadFloat());
	ReadSemicolon();

	return color;
}

ColorF MeshLoader_X::ReadRGB()
{
	ColorF color;
	color.Set(ReadFloat(), ReadFloat(), ReadFloat(), 0);
	ReadSemicolon();

	return color;
}