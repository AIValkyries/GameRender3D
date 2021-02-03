#pragma once
#ifndef __Camera_H__
#define __Camera_H__

#include "ISceneNode.h"
#include "DataType.h"
#include "Geometry.h"

#define SCREEN_WIDTH        860  // size of screen
#define SCREEN_HEIGHT       600

class Camera :public ISceneNode
{
public:
	virtual ISceneNode* Create(GameHost* _gameHost);
	virtual void Destroy();
	virtual void PreUpdate();
	virtual void Update(bool updateChild);
	virtual void UpdateBox(bool updateChild);
	virtual void Render(bool updateChild);
	virtual void RenderBox(bool updateChild);
	virtual MeshBuffer* GetMeshBuffer() { return 0; }

	void CreateDefaultCamera(GameHost* _gameHost);
	void CreateCamera(
		const float32& _fov, const float32& _nearPlane, const float32& _farPlane,
		const float32& viewWidth, const float32& viewHegiht,
		const Vector3& pos, const Vector3& s, const Quaternion& q,
		GameHost* _gameHost);
	void CreateCamera(
		const float32& _fov, const float32& _nearPlane, const float32& _farPlane,
		const float32& viewWidth, const float32& viewHegiht,
		const Vector3& pos, const Vector3& s, const Vector3& r,
		GameHost* _gameHost);

	// 投影矩阵
	void SetProjParams(
		float32& _fov,
		float32& _aspect,
		float32& _n,
		float32& _f);

	void SetOrthoParams(
		float32& left, float32& right,
		float32& _w, float32& _h,
		float32& _n, float32& _f);

	float32 GetFov()const { return fov; }
	float32 GetAspect()const { return aspect; }
	float32 GetWidth()const { return width; }
	float32 GetHeight()const { return height; }
	float32 GetNearPlane()const { return nearPlane; }
	float32 GetFarPlane()const { return  farPlane; }
	float32 GetViewDistance()const { return farPlane - nearPlane; }

	Matrix4x4 GetProjMatrix4x4()const { return projMat; }
	Matrix4x4 GetViewProjMatrix4x4()const { return viewProjMat; }
	Matrix4x4 GetInvViewProjMatrix4x4()const { return  invViewProjMat; }
	Matrix4x4 GetViewMatrix4x4()const { return viewMat; }

	const AABB* GetAABB()const
	{
		if (frustum) return frustum->GetBoundingBox();
		return 0; 
	}
	Vector3* GetFrustumPlanePoints()const { return frustumPlanePoints; }
	Frustum* GetFrustum()const { return frustum; }

	bool IsOrthogonal()const { return  isOrthogonal; }
	bool IsPerspective()const { return isPerspective; }

private:

	Matrix4x4 projMat;
	Matrix4x4 viewProjMat;
	Matrix4x4 viewMat;

	// 视图投影的逆矩阵
	Matrix4x4 invViewProjMat;

	Frustum* frustum;
	Vector3* frustumPlanePoints; // 视锥体的8个顶点

	float32 fov;
	float32 aspect;
	float32 width;
	float32 height;
	float32 nearPlane;
	float32 farPlane;

	bool isOrthogonal;
	bool isPerspective;

	// 1.计算视图矩阵
    // 2.计算投影视图矩阵
    // 3.计算视锥体的 AABB
    // 4.计算视锥体   Frustum
    // 5.计算天空盒   TODO
	// 立方体的8个顶点
	//    p0----p3
	//   /     /|
	//  p4----p7|
	//  | p1  | p2
	//  |     |/
	//  p5----p6
	void BuildViewProjMatrix();

};

#endif

