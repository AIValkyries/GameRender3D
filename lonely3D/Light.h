#pragma once
#ifndef __Light_H__
#define __Light_H__

#include "DataType.h"
#include "Material.h"
#include "Vertex.h"
#include "lMathf.h"
#include <vector>
#include "UtilTools.h"
#include "Types.h"
#include "LightSceneNode.h"

#define MAX_LIGHT_COUNT 8
#define LIGHT_DEFINE_RADIUS 120  // ����Ĭ�ϰ뾶
#define CONSTANY_ATTENUATION  1
#define LINER_ATTENUATION 0.002F
#define QUADRATIC_ATTENUATION 0.0005F

#define OUTER_CONE  60
#define INNER_CONE 15

class Light
{
public:
	Light() :
		AmbientColor(0),
		DiffuseColor(0),
		SpecularColor(0),
		Type(L_DIRECTIONAL),
		Direction(Vector3::zero()),
		Position(Vector4::zero()),
		Radius(0),
		ConstantAttenuation(1), LinerAttenuation(0.002F), QuadraticAttenuation(0),
		OuterCone(60.0F), InnerCone(30.0F), Falloff(2.0F)
	{}

	ColorF AmbientColor;
	ColorF DiffuseColor;
	ColorF SpecularColor;

	LIGHY_SOURCE_TYPE Type;
	Vector3 Direction;  // ���Դû�з���
	Vector4 Position;   // ƽ�е�û��λ��

	float32 Radius;
	// ��������
	float32 ConstantAttenuation;
	float32 LinerAttenuation;
	float32 QuadraticAttenuation;

	float32 OuterCone;
	float32 InnerCone;
	float32 Falloff;
};

class LightSpace
{
public:
	Light* CreateDirectionalLight(
		ColorF& ambientColor,
		ColorF& diffuseColor,
		ColorF& specularColor,
		Vector3& direction);

	Light* CreatePointLight(
		ColorF& ambientColor,
		ColorF& diffuseColor,
		ColorF& specularColor,
		Vector3& position,
		float32 radius,
		float32 constantAttenuation = CONSTANY_ATTENUATION,
		float32 linerAttenuation = LINER_ATTENUATION,
		float32 quadraticAttenuation = QUADRATIC_ATTENUATION);

	Light* CreateStandardSpotLight(
		ColorF& ambientColor,
		ColorF& diffuseColor,
		ColorF& specularColor,
		Vector3& position,
		Vector3& direction,
		float32 falloff,
		float32 radius,
		float32 constantAttenuation = CONSTANY_ATTENUATION,
		float32 linerAttenuation = LINER_ATTENUATION,
		float32 quadraticAttenuation = QUADRATIC_ATTENUATION,
		float32 outerCone = OUTER_CONE,
		float32 innerCone = INNER_CONE);

	void SetCameraPosition(const Vector4& _camePos)
	{
		camWorldPos = _camePos;
	}

	void ApplyLight(Vertex* vertex, MeshBuffer* meshBuffer);
	void ApplyLight(MeshBuffer* meshBuffer);
	void PushBack(LightSceneNode* lightNode)
	{
		if (lightNode) lightNodes.push_back(lightNode);
	}

	int32 GetLightCount()const { return lightNodes.size(); }

private:
	std::vector<LightSceneNode*> lightNodes;
	Vector4 camWorldPos;
};


// ƽ�е�
inline Light* LightSpace::CreateDirectionalLight(
	ColorF& ambientColor,
	ColorF& diffuseColor,
	ColorF& specularColor,
	Vector3& direction)
{
	Light* light = new Light();

	light->AmbientColor  = ambientColor;
	light->DiffuseColor  = diffuseColor;
	light->SpecularColor = specularColor;
	light->Direction     = direction;
	light->Type = L_DIRECTIONAL;

	return light;
}

inline Light* LightSpace::CreatePointLight(
	ColorF& ambientColor,
	ColorF& diffuseColor,
	ColorF& specularColor,
	Vector3& position,
	float32 radius,
	float32 constantAttenuation,
	float32 linerAttenuation,
	float32 quadraticAttenuation)
{
	Light* light = new Light();
	light->AmbientColor = ambientColor;
	light->DiffuseColor = diffuseColor;
	light->SpecularColor = specularColor;
	light->Position = position;
	light->Radius = radius;
	light->Type = L_POINT;

	light->ConstantAttenuation = constantAttenuation;
	light->LinerAttenuation = linerAttenuation;
	light->QuadraticAttenuation = quadraticAttenuation;

	return light;
}

// �۹��
inline Light* LightSpace::CreateStandardSpotLight(
	ColorF& ambientColor,
	ColorF& diffuseColor,
	ColorF& specularColor,
	Vector3& position,
	Vector3& direction,
	float32 falloff,
	float32 radius,
	float32 constantAttenuation,
	float32 linerAttenuation,
	float32 quadraticAttenuation,
	float32 outerCone ,
	float32 innerCone)
{
	Light* light = new Light();
	light->AmbientColor = ambientColor;
	light->DiffuseColor = diffuseColor;
	light->SpecularColor = specularColor;
	light->Type = L_STANDARD_SPOT;

	light->Position = position;
	light->Direction = direction;
	light->Falloff = falloff;

	light->ConstantAttenuation = constantAttenuation;
	light->LinerAttenuation = linerAttenuation;
	light->QuadraticAttenuation = quadraticAttenuation;
	light->Radius = radius;

	light->OuterCone = outerCone;
	light->InnerCone = innerCone;
	light->Falloff = falloff;

	return light;
}

inline void LightSpace::ApplyLight(MeshBuffer* meshBuffer)
{
	if (meshBuffer == 0) return;
	if (lightNodes.size() == 0)return;

	int32 vertexCount = meshBuffer->GetVertexCount();

	for (int32 i = 0;i < vertexCount;i++)
	{
		Vertex* transformVertex = meshBuffer->GetTransformVertex(i);
		ApplyLight(transformVertex, meshBuffer);
	}
}

// ������ռ��н��м���
// mdiff=������������ɫ sdiff��Դ��������ɫ
// idiff = (Max(0,Dot(n,l)) * mdiff * sdiff)
// ispec = (Max(0,Dot(r,v)))*mspec*sspec  v=�ӽ� r = reflection(l,n)
// ispec = (Max(0,Dot(v,h)))*mspec*sspec h = (1+v)/(|l+v|)
// iamb=mamb*samb
inline void LightSpace::ApplyLight(Vertex* vertex, MeshBuffer* meshBuffer)
{
	lMaterial* material = meshBuffer->GetMaterial();

	if (lightNodes.size() <= 0)
	{
		if (material)
			vertex->color.AddEqual(material->EmissiveColor);

		return;
	}

	ColorF totalColor;
	totalColor += vertex->color;

	if (material) totalColor += material->EmissiveColor;

	ColorF ambientColor;
	ColorF diffuseColor;
	ColorF specColor;

	float32 minDist = MAX_FLOAT_32;
	float32 minAngle = MAX_FLOAT_32;
	float32 maxAngle = MIN_FLOAT_32;

	for (uint32 i = 0;i < lightNodes.size();i++)
	{
		Light* light = lightNodes[i]->GetLight();
		if (meshBuffer->CheckRenderState(AMBIENT_LIGHT))  // ������
			ambientColor += light->AmbientColor;

		Vector4 lightPos = lightNodes[i]->GetWorldPosition();

		switch (light->Type)
		{
		case L_POINT:
		{
			// �жϾ����Ƿ���Ч
			float32 dist = Distance(lightPos, vertex->position);
			if (dist > light->Radius)
			{
				if (minDist > dist)
					minDist = dist;
				continue;
			}

			Vector4 l = lightPos - vertex->position;
			l.Normalize();

			float32 linerAtt = light->LinerAttenuation * dist;
			float32 quadraticAtt = light->QuadraticAttenuation * (dist * dist);

			// ˥����������֮��ľ���
			float32 attenuation = 1.0F / (light->ConstantAttenuation + linerAtt + quadraticAtt);

			if (meshBuffer->CheckRenderState(DIFFUSE_LIGHT))
			{
				// ���淴���������
				float32 dot = Max(0, l.Dot(vertex->normal));
				if (dot > 0.0F)
					diffuseColor.AddEqual(light->DiffuseColor * (dot * attenuation));
			}

			if (meshBuffer->CheckRenderState(SPECULAR_LIGHT))
			{
				// ����Dot(v,r) ����Dot(v,h)��?���Ǻ��߰�
				Vector4 v = camWorldPos - vertex->position;
				v.Normalize();
				Vector3 r = Reflect(l, vertex->normal);
				r.Normalize();

				float32 power = material ? material->Power : 2;
				float32 dot = Max(r.Dot(v), 0);

				if (dot > 0)
				{
					float32 mshi = Pow(dot, power);
					float32 scale = mshi * attenuation;

					specColor.AddEqual(light->SpecularColor * scale);
				}
			}
		}break;
		case L_STANDARD_SPOT:
		{
			// ��Ч�ľ���
			float32 dist = Distance(vertex->position, lightPos);
			if (dist > light->Radius)
			{
				if (minDist > dist)
					minDist = dist;
				continue;
			}

			// �����
			Vector4 l = vertex->position - lightPos;
			l.Normalize();
			float32 halfOuterCone = light->OuterCone * 0.5F;

			//�����붥�㷽��ļн�,�����������򲻼���
			float32 angle = vertex->normal.Dot(l);
			if (angle < 0)
			{
				if (minAngle > angle)
					minAngle = angle;
				if (maxAngle < angle)
					maxAngle = angle;
				continue;
			}

			float32 linerAtt = light->LinerAttenuation * dist;
			float32 quadraticAtt = light->QuadraticAttenuation * (dist * dist);

			float32 attenuation = 1.0F / (light->ConstantAttenuation + linerAtt + quadraticAtt);

			float32 halfInnerCone = light->InnerCone * 0.5F;

			float32 falloff = 1;

			// ����ڰ�Ӱ��֮��
			if (angle > halfInnerCone && angle < halfOuterCone)
			{
				float32 innerCone = (float32)(halfInnerCone * DEG_TO_RAD);
				float32 outerCone = (float32)(halfOuterCone * DEG_TO_RAD);

				float32 molecular = Cos((float32)(angle * DEG_TO_RAD)) - Cos(outerCone);
				float32 denominator = Cos(innerCone) - Cos(outerCone);

				falloff = Pow(molecular, light->Falloff) / denominator;
			}

			if (meshBuffer->CheckRenderState(DIFFUSE_LIGHT))
			{
				float32 dot = Max(0, l.Dot(vertex->normal));
				if (dot > 0.0F)
					diffuseColor.AddEqual(light->DiffuseColor * dot * falloff);
			}

			if (meshBuffer->CheckRenderState(SPECULAR_LIGHT))
			{
				// Dot(v*h)
				Vector4 v = camWorldPos - vertex->position;
				v.Normalize();
				Vector3 r = Reflect(l, vertex->normal);

				float32 power = material ? material->Power : 2;
				float32 dot = Max(r.Dot(v), 0);

				if (dot > 0)
				{
					float32 mshi = Pow(dot, power);
					specColor.AddEqual(
						light->SpecularColor* mshi* attenuation* falloff);
				}
			}

		}break;
		case L_DIRECTIONAL:  // �����û��˥����Ҳû��λ��
		{
			if (meshBuffer->CheckRenderState(DIFFUSE_LIGHT))
			{
				float32 dot = Max(0, light->Direction.Dot(vertex->normal));
				if (dot > 0.0F)
					diffuseColor.AddEqual(light->DiffuseColor * dot);
			}
			//if (meshBuffer->CheckRenderState(SPECULAR_LIGHT))
			//{
			//	Vector4 v = camWorldPos - vertex->position; // �ӵ㷽��
			//	v.Normalize();
			//	Vector3 r = Reflect(light->Direction, vertex->normal);   // ��Դ����

			//	float32 power = material ? material->Power : 2;
			//	float32 dot = Max(r.Dot(v), 0);

			//	if (dot > 0)
			//	{
			//		float32 mshi = Pow(dot, power);
			//		specColor.AddEqual(light->SpecularColor * mshi);
			//	}
			//}

		}break;
		}
	}

	if (material)
	{
		totalColor.AddEqual(ambientColor * material->AmbientColor);
		totalColor.AddEqual(diffuseColor * material->DiffuseColor);
		totalColor.AddEqual(specColor * material->SpecularColor);
	}
	else
	{
		totalColor.AddEqual(ambientColor * vertex->color);
		totalColor.AddEqual(diffuseColor * vertex->color);
		totalColor.AddEqual(specColor * vertex->color);
	}

	totalColor.Saturate();
	vertex->color = totalColor;

	//Debug::LogError("-------------------------------------------");
	////Debug::LogError("��С����=" + Convert(minDist));
	//Debug::LogError("��С�Ƕ�=" + Convert(minAngle));
	//Debug::LogError("���Ƕ�=" + Convert(maxAngle));
}

#endif
