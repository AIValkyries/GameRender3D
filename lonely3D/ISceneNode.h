#pragma once
#ifndef __ISceneNode_H__
#define __ISceneNode_H__

#include "DataType.h"
#include "lMathf.h"
#include "Geometry.h"
#include "BitFlags.h"
#include <vector>
#include "MeshBuffer.h"
#include "Types.h"

// 节点ID
static int32 NODE_TOTAL_ID = 0;
static int32 transparent_ID = 10000;  // 透明通道的ID

class GameHost;

class ISceneNode
{
public:
	ISceneNode();
	~ISceneNode();

	virtual ISceneNode* Create(GameHost* _gameHost) = 0;
	virtual void PreUpdate() = 0;	
	virtual void Update(bool updateChild) = 0;         // 每帧更新节点
	virtual void UpdateBox(bool updateChild) = 0;
	virtual void Render(bool updateChild) = 0;
	virtual void RenderBox(bool updateChild) = 0;
	virtual void Destroy() = 0;

	bool IsAChild(const ISceneNode* child)const;
	bool IsCanUpdateMatrix();

	void SetLocalToWorldMatrix4x4(Matrix4x4& mat);
	void SetLocalToWorldMatrix3x3(Matrix3x3& mat);

	// 都是局部位置，方法名就不改了
	void SetPosition(const Vector3& _position);
	void SetPosition(const Vector4& _position);
	void SetPosition(float32 x, float32 y, float32 z);
	void SetScale(const Vector3& _scale);
	void SetScale(const float32& scale);
	void SetScale(float32 x, float32 y, float32 z);
	void SetRotate(const Quaternion& _rotate);
	void SetRotate(const Vector3& _rotate);
	void SetRotate(float32 x, float32 y, float32 z);

	Vector4& Translate(Vector3 _position);
	Vector4& Translate(Vector4 _position);
	void AddScale(Vector3 scale);
	void SetRotateX(float32 angle);
	void SetRotateY(float32 angle);
	void SetRotateZ(float32 angle);

	void AttachToParent(ISceneNode* _parent);
	void DetachFromParent();
	void PromoteChildren();   // 将孩子提升为兄弟
	void RelocateChildren(ISceneNode* _parent);  // 孩子节点转移

	void UpdateMatrix();

	ISceneNode* Parent()const           { return parent; }
	vector<ISceneNode*> Childs()  const { return children; }
	vector<ISceneNode*> Sibling() const { return sibling; }
	vector<ISceneNode*> GetAllNodes();

	int32 ChildCount()   const   { return children.size(); }
	int32 SiblingCount() const   { return sibling.size(); }

	bool HasParent()  const { return parent != 0; }
	bool HasChild()   const { return children.size() != 0; }
	bool HasSibling() const { return sibling.size() != 0; }

	Vector4 GetWorldPosition()const;
	Vector3 GetWorldScale()const;
	Vector3 GetWorldEulerAngle()const;

	Vector4 GetLocalPosition()const { return localPosition; }
	Vector3 GetLocalScale()const { return localScale; }
	Quaternion GetLocalRotate()const { return localRotation; }
	Vector3 GetLocalEulerAngle()
	{ 
		QuaternionToEuler(localRotation, eulerAngle);
		return eulerAngle;
	}
	Matrix4x4 GetGlobaLocalToWorldMatrix()const { return globaLocalToWorldMatrix; }
	Matrix4x4 GetGlobaWorldToLocalMatrix()const { return globaWorldToLocalMatrix; }
	Matrix4x4 GetLocalToWorldMatrix()const { return localToWorldMatrix; }
	Matrix4x4 GetWorldToLocalMatrix()const { return worldToLocalMatrix; }

	virtual vector<MeshBuffer*> GetMeshBuffers()const { return meshBuffers; }

	void ClearPosition() { localPosition = Vector4::zero(); }
	void ClearRotation() { localRotation = Quaternion::identity(); }
	void ClearScale()    { localScale = Vector3::zero(); }

	void SetName(String _name) { name = _name; }
	void SetRenderState(RENDER_STATE_TYPE State, uint32 value);
	void SetTexture(int32 bufferID, Texture* texture);
	void SetMaterial(lMaterial* _material);
	void SetMaterial(int32 bufferID, lMaterial* _material);
	void SetVisible(bool isVisible);

	bool IsVisible()const { return isVisible; }
	uint32 GetNodeID()const { return id; }
	AABB* GetWorldBounds()const { return worldBounds; }

	const float32 GetQueueSortValue()const { return queueSortValue; }

	void SetVertexColor(const ColorF& c);
	void SetTextureMulColor(bool _textureMulColor)
	{
		textureMulColor = _textureMulColor;
	}

	void SetShaderType(SHADER_TYPE type)
	{
		shaderType = type;
	}

	int32 GetShaderType()const { return shaderType; }

protected:

	GameHost* gameHost;
	// 渲染排序
	float32 queueSortValue;
	bool textureMulColor;
	int32 shaderType;

	AABB* worldBounds;
	vector<MeshBuffer*> meshBuffers;
	String name;
	uint32 id;          // 节点ID,唯一性的
	ISceneNode* parent;
	vector<ISceneNode*> children;
	vector<ISceneNode*> sibling;

	Matrix4x4 globaLocalToWorldMatrix;
	Matrix4x4 globaWorldToLocalMatrix;
	Matrix4x4 localToWorldMatrix;  // 局部 2 世界矩阵
	Matrix4x4 worldToLocalMatrix;  // 世界 2 局部矩阵

	// 局部坐标
	Vector4 localPosition;
	Vector3 localScale;
	Quaternion localRotation;
	Vector3 eulerAngle;

	u8Flags updateFlags;

	bool isVisible;
	bool isDebugObject;

	void AddChild(ISceneNode* node);
	void AddSibling(ISceneNode* node);
	void RemoveChild(ISceneNode* node);
	void RemoveSibling(ISceneNode* node);

};

inline ISceneNode::ISceneNode()
{
	id = NODE_TOTAL_ID++;
	isVisible = true;
}

inline ISceneNode::~ISceneNode()
{
	
}

inline void ISceneNode::SetVertexColor(const ColorF& c)
{
	if (meshBuffers.size() == 0)
		return;

	for (uint32 i = 0;i < meshBuffers.size();i++)
	{
		MeshBuffer* mb = meshBuffers[i];

		int32 vertexCount = mb->GetVertexCount();

		for (int32 i = 0;i < vertexCount;i++)
		{
			mb->GetSourceVertex(i)->color = c;
		}
	}

}

inline bool ISceneNode::IsAChild(const ISceneNode* child)const
{
	for (uint32 i = 0;i < children.size();i++)
	{
		if (children[i]->id == child->id)
			return true;
	}

	return false;
}

inline void ISceneNode::SetPosition(const Vector3& _position)
{
	localPosition = _position;
	updateFlags.SetBit(UpdatePosition);
}

inline void ISceneNode::SetPosition(const Vector4& _position)
{
	localPosition = _position;
	updateFlags.SetBit(UpdatePosition);
}

inline void ISceneNode::SetPosition(float32 x, float32 y, float32 z)
{
	localPosition.x = x;
	localPosition.y = y;
	localPosition.z = z;
	localPosition.w = 1;
	updateFlags.SetBit(UpdatePosition);
}

inline void ISceneNode::SetScale(const float32& scale)
{
	localScale = Vector3(scale, scale, scale);
	updateFlags.SetBit(UpdateScale);
}

inline void ISceneNode::SetScale(const Vector3& _scale)
{
	localScale = _scale;
	updateFlags.SetBit(UpdateScale);
}

inline void ISceneNode::SetScale(float32 x, float32 y, float32 z)
{
	localScale.x = x;
	localScale.y = y;
	localScale.z = z;
	updateFlags.SetBit(UpdateScale);
}

inline void ISceneNode::SetRotate(const Quaternion& _rotate)
{
	localRotation = _rotate;
	updateFlags.SetBit(UpdateRotate);
}

inline void ISceneNode::SetRotate(const Vector3& _rotate)
{
	eulerAngle += _rotate;
	EulerToQuaternion(eulerAngle, localRotation);
	updateFlags.SetBit(UpdateRotate);
}

inline void ISceneNode::SetRotate(float32 x, float32 y, float32 z)
{
	eulerAngle.x += x;
	eulerAngle.y += y;
	eulerAngle.z += z;
	EulerToQuaternion(eulerAngle, localRotation);
	updateFlags.SetBit(UpdateRotate);
}

inline void ISceneNode::SetRotateX(float32 angle)
{
	eulerAngle.x += angle;
	EulerToQuaternion(eulerAngle, localRotation);
	updateFlags.SetBit(UpdateRotate);
}

inline void ISceneNode::SetRotateY(float32 angle)
{
	eulerAngle.y += angle;
	EulerToQuaternion(eulerAngle, localRotation);
	updateFlags.SetBit(UpdateRotate);
}

inline void ISceneNode::SetRotateZ(float32 angle)
{
	eulerAngle.z += angle;
	EulerToQuaternion(eulerAngle, localRotation);
	updateFlags.SetBit(UpdateRotate);
}

inline Vector4& ISceneNode::Translate(Vector3 _position)
{
	localPosition += _position;
	updateFlags.SetBit(UpdatePosition);
	return localPosition;
}

inline Vector4& ISceneNode::Translate(Vector4 _position)
{
	localPosition += _position;
	updateFlags.SetBit(UpdatePosition);
	return localPosition;
}

inline void ISceneNode::AddScale(Vector3 _scale)
{
	localScale += _scale;
	updateFlags.SetBit(UpdateScale);
}


inline void ISceneNode::UpdateMatrix()
{
	if (IsCanUpdateMatrix() || (parent != 0 && parent->IsCanUpdateMatrix()))
	{
		QuaternionToMatrix(localRotation, localToWorldMatrix);
		QuaternionToMatrix(localRotation.Conjugation(), worldToLocalMatrix);
	
		localToWorldMatrix[0] *= localScale.x;
		localToWorldMatrix[1] *= localScale.y;
		localToWorldMatrix[2] *= localScale.z;

		localToWorldMatrix[4] *= localScale.x;
		localToWorldMatrix[5] *= localScale.y;
		localToWorldMatrix[6] *= localScale.z;

		localToWorldMatrix[8] *= localScale.x;
		localToWorldMatrix[9] *= localScale.y;
		localToWorldMatrix[10] *= localScale.z;

		float32 fScalex = 1.0F / localScale.x;
		float32 fScaley = 1.0F / localScale.y;
		float32 fScalez = 1.0F / localScale.z;

		worldToLocalMatrix[0] *= fScalex;
		worldToLocalMatrix[1] *= fScaley;
		worldToLocalMatrix[2] *= fScalez;

		worldToLocalMatrix[4] *= fScalex;
		worldToLocalMatrix[5] *= fScaley;
		worldToLocalMatrix[6] *= fScalez;

		worldToLocalMatrix[8] *= fScalex;
		worldToLocalMatrix[9] *= fScaley;
		worldToLocalMatrix[10] *= fScalez;

		localToWorldMatrix[3] = localPosition.x;
		localToWorldMatrix[7] = localPosition.y;
		localToWorldMatrix[11] = localPosition.z;
		localToWorldMatrix[15] = 1.0F;

		Vector3 pos = -Vector3(localPosition.x, localPosition.y, localPosition.z);
		Matrix4x4 positionMat;
		positionMat.SetIdentity();
		positionMat.SetPosition(pos);

		worldToLocalMatrix *= positionMat;

		if (parent != 0)
		{
			Matrix4x4 parentMatrix = parent->GetGlobaLocalToWorldMatrix();

			MultiplyMatrix(
				globaLocalToWorldMatrix, parentMatrix, localToWorldMatrix);
		}
		else
			globaLocalToWorldMatrix = localToWorldMatrix;

		if (parent != 0)
		{
			Matrix4x4 parentworldMatrix = parent->GetGlobaWorldToLocalMatrix();

			MultiplyMatrix(
				globaWorldToLocalMatrix,parentworldMatrix,worldToLocalMatrix);
		}
		else
			globaWorldToLocalMatrix = worldToLocalMatrix;
	}

}
 
inline bool ISceneNode::IsCanUpdateMatrix()
{
	if (updateFlags.TestBit(UpdatePosition)
		|| updateFlags.TestBit(UpdateRotate)
		|| updateFlags.TestBit(UpdateScale)
		|| updateFlags.TestBit(UpdateAll))
	{
		return true;
	}

	return false;

}


inline void ISceneNode::AddChild(ISceneNode* node)
{
	children.push_back(node);
}

inline void ISceneNode::AddSibling(ISceneNode* node)
{
	sibling.push_back(node);
}

inline void ISceneNode::RemoveChild(ISceneNode* node)
{
	std::vector<ISceneNode*>::iterator itr;

	for (itr = children.begin();itr != children.end();itr++)
	{
		if ((*itr) == node)
		{
			children.erase(itr);
			break;
		}
	}
}


inline void ISceneNode::RemoveSibling(ISceneNode* node)
{
	std::vector<ISceneNode*>::iterator itr;

	for (itr = sibling.begin();itr != sibling.end();itr++)
	{
		if ((*itr) == node)
		{
			sibling.erase(itr);
			break;
		}
	}
}

inline void ISceneNode::AttachToParent(ISceneNode* _parent)
{
	if (_parent != parent)
	{
		DetachFromParent();
		parent = _parent;
		parent->AddChild(this);
	}
}

inline void ISceneNode::DetachFromParent()
{
	if (parent != 0)
	{
		parent->RemoveChild(this);
		parent = 0;
	}
}

// 将孩子提升为兄弟
inline void ISceneNode::PromoteChildren()
{
	for (uint32 i = 0;i < children.size();i++)
	{
		AddSibling(children[i]);
	}
}

// 孩子节点转移
inline void ISceneNode::RelocateChildren(ISceneNode* _parent)
{
	if (_parent != this)
	{
		for (uint32 i = 0;i < children.size();i++)
		{
			_parent->AddChild(children[i]);
		}
		children.clear();
	}
}

inline void ISceneNode::SetRenderState(RENDER_STATE_TYPE State, uint32 value)
{
	if (meshBuffers.size())
	{
		if (State == TRANSPARENTT)
		{
			if (value == 1)
			{
				queueSortValue = float32(transparent_ID++);
			}
			else
			{
				queueSortValue = 0;
			}
		}

		for (uint32 i = 0;i < meshBuffers.size();i++)
		{
			meshBuffers[i]->SetRenderState(State, value);
		}
	}
}

inline void ISceneNode::SetMaterial(int32 bufferID, lMaterial* _material)
{
	if ((uint32)bufferID < meshBuffers.size())
	{
		meshBuffers[bufferID]->SetMaterial(_material);
	}
}

inline void ISceneNode::SetMaterial(lMaterial* _material)
{
	if (meshBuffers.size())
	{
		for (uint32 i = 0;i < meshBuffers.size();i++)
		{
			meshBuffers[i]->SetMaterial(_material);
		}
	}
}

inline void ISceneNode::SetTexture(int32 bufferID, Texture* texture)
{
	if ((uint32)bufferID < meshBuffers.size())
	{
		meshBuffers[bufferID]->SetTexture(texture);
	}
}

inline void ISceneNode::SetLocalToWorldMatrix4x4(Matrix4x4& mat)
{
	localToWorldMatrix = mat;

	if (parent != 0)
	{
		MultiplyMatrix(
			globaLocalToWorldMatrix,
			parent->GetGlobaLocalToWorldMatrix(),
			localToWorldMatrix);
	}
	else
		globaLocalToWorldMatrix = localToWorldMatrix;
}

inline void ISceneNode::SetLocalToWorldMatrix3x3(Matrix3x3& mat)
{
	localToWorldMatrix = mat;

	if (parent != 0)
	{
		MultiplyMatrix(
			globaLocalToWorldMatrix,
			parent->GetGlobaLocalToWorldMatrix(),
			localToWorldMatrix);
	}
	else
		globaLocalToWorldMatrix = localToWorldMatrix;
}

inline vector<ISceneNode*> ISceneNode::GetAllNodes()
{
	vector<ISceneNode*> allNodes;
	allNodes.push_back(this);

	if (children.size())
	{
		for (uint32 i = 0;i < children.size();i++)
		{
			vector<ISceneNode*> childs = children[i]->GetAllNodes();
			if (childs.size())
			{
				for (uint32 j = 0;j < childs.size();j++)
				{
					allNodes.push_back(childs[j]);
				}
			}
		}
	}

	return allNodes;
}

inline void ISceneNode::SetVisible(bool _isVisible)
{
	isVisible = _isVisible;
}

inline Vector4 ISceneNode::GetWorldPosition()const
{
	Vector4 position;
	position.x = globaLocalToWorldMatrix[3];
	position.y = globaLocalToWorldMatrix[7];
	position.z = globaLocalToWorldMatrix[11];
	position.w = globaLocalToWorldMatrix[15];

	return position;
}

inline Vector3 ISceneNode::GetWorldScale()const
{
	return globaLocalToWorldMatrix.GetScale();
}

inline Vector3 ISceneNode::GetWorldEulerAngle()const
{
	return globaLocalToWorldMatrix.GetRotate();
}



#endif

