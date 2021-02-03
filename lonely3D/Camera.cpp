#include "stdafx.h"
#include "Camera.h"
#include "Debug.h"
#include "SceneObject.h"
#include "UtilTools.h"
#include "GameHost.h"

void Camera::CreateDefaultCamera(GameHost* _gameHost)
{
	float32 fov = 60;
	float32 nearD = 50;
	float32 farD = 500;
	float32 width = SCREEN_WIDTH;
	float32 height = SCREEN_HEIGHT;
	Vector3 pos(0, 0, -50);
	Vector3 scale(1, 1, 1);
	Vector3 rotate(0, 0, 0);

	CreateCamera(
		fov,
		nearD, farD,
		width, height,
		pos,
		scale,      // 缩放
		rotate,
		_gameHost);  // 旋转

	// 视图投影矩阵,初始化
	MultiplyMatrix(viewProjMat, projMat, viewMat);
	frustum->ExtractFromMatrix(viewProjMat);
}

void Camera::CreateCamera(
	const float32& _fov, const float32& _nearPlane, const float32& _farPlane,
	const float32& viewWidth, const float32& viewHegiht,
	const Vector3& pos, const Vector3& s, const Vector3& r,
	GameHost* _gameHost)
{
	Create(_gameHost);

	fov = _fov;
	nearPlane = _nearPlane;
	farPlane = _farPlane;
	width = viewWidth;
	height = viewHegiht;
	aspect = (float32)width / (float32)height;

	SetPosition(pos);
	if (r != Vector3::zero())
	{
		EulerToQuaternion(eulerAngle, localRotation);
		updateFlags.SetBit(UpdateRotate);
	}
	SetScale(s);

	isOrthogonal = false;
	isPerspective = false;

	UpdateMatrix();
	SetProjParams(fov, aspect, nearPlane, farPlane);
	BuildViewProjMatrix();

	frustumPlanePoints = new Vector3[8];
}

void Camera::CreateCamera(
	const float32& _fov, const float32& _nearPlane, const float32& _farPlane,
	const float32& viewWidth, const float32& viewHegiht,
	const Vector3& pos, const Vector3& s, const Quaternion& q,
	GameHost* _gameHost)
{
	Create(_gameHost);

	fov = _fov;
	nearPlane = _nearPlane;
	farPlane = _farPlane;
	width = viewWidth;
	height = viewHegiht;
	aspect = (float32)width / (float32)height;

	SetPosition(pos);
	SetRotate(q);
	SetScale(s);

	isOrthogonal = false;
	isPerspective = false;

	UpdateMatrix();
	SetProjParams(fov, aspect, nearPlane, farPlane);
	BuildViewProjMatrix();

	frustumPlanePoints = new Vector3[8];
}
 

inline void Camera::BuildViewProjMatrix()
{
	MultiplyMatrix(
		viewProjMat,
		localToWorldMatrix,
		projMat);
}

ISceneNode* Camera::Create(GameHost* _gameHost)
{
	parent = 0;
	globaLocalToWorldMatrix.SetIdentity();
	globaWorldToLocalMatrix.SetIdentity();
	localToWorldMatrix.SetIdentity();
	worldToLocalMatrix.SetIdentity();

	localPosition = Vector4::zero();
	localScale = Vector3::zero();
	localRotation = Quaternion::identity();

	projMat.SetIdentity();
	viewProjMat.SetIdentity();
	viewMat.SetIdentity();

	frustum = new Frustum();

	gameHost = _gameHost;

	return this;
}

void Camera::Render(bool updateChild)
{

}

void Camera::RenderBox(bool updateChild)
{

}

void Camera::PreUpdate()
{

}

//lateralSpeed 10.0f,
//rotationSpeed 0.1f,
//pTerrain& m_terrainSystem,
//groundOffset 30.0f,
//forceUpdate true
void Camera::Update(bool updateChild)
{
	static float32 rotationSpeed = 0.1F;
	static float32 lateralSpeed = 10.0f;
	static float32 zSpeed = 1.0F;

	if (frustum) frustum->SetCameraPosition(localPosition);

	UpdateMatrix();
	viewMat = globaWorldToLocalMatrix;

	if (IsCanUpdateMatrix())
	{
		// 视图投影矩阵
		MultiplyMatrix(viewProjMat, projMat, viewMat);
		frustum->ExtractFromMatrix(viewProjMat, true);
	}

	updateFlags.ClearBitAll();

	gameHost->GetLightSpace()->SetCameraPosition(GetWorldPosition());
}

void Camera::UpdateBox(bool updateChild)
{

}

void Camera::Destroy()
{
	Debug::LogInfo("移除相机内存!!");
	SafeDeleteArray(frustumPlanePoints);
	SafeDelete(frustum);
}

void Camera::SetProjParams(
	float32& _fov, 
	float32& _aspect,
	float32& _n, 
	float32& _f)
{
	fov = _fov;
	aspect = _aspect;
	nearPlane = _n;
	farPlane = _f;

	projMat = MatrixPerspectiveFovLH(fov, aspect, _n, _f);

	isPerspective = true;
}

void Camera::SetOrthoParams(
	float32& left,float32& right,
	float32& _w, float32& _h,
	float32& _n, float32& _f)
{
	width = _w;
	height = _h;
	nearPlane = _n;
	farPlane = _f;

	projMat = MatrixOrthoLH(left, right, fov, aspect, _n, _f);

	isOrthogonal = true;
}
