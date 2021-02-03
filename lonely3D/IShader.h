#pragma once
#ifndef __IShader_H__
#define __IShader_H__

#include "DataType.h"
#include "ZBuffer.h"
#include "Vertex.h"
#include "BitFlags.h"
#include "Geometry.h"
#include "Color.h"
#include "Types.h"
#include "Texture.h"
#include "FixedPoint.h"

#define TRI_TYPE_NONE           0 
#define TRI_TYPE_BOTTOM         1
#define TRI_TYPE_TOP            2
#define TRI_TYPE_MASK           3
#define TRI_TYPE_FLAT_GENERAL   4

typedef struct
{
	float32 a;
	float32 r;
	float32 g;
	float32 b;

	float32 x;
	float32 zValue;

	float32 u;
	float32 v;
}Vertex_Tri;

typedef struct
{
	Vertex_Tri left;
	Vertex_Tri right;
	Vertex_Tri step;
	Vertex_Tri v;

}Trapezioid;


class IShader
{
public:
	IShader();
	~IShader();

	void SetRenderTarget(Texture* _renderTarget)
	{
		if (_renderTarget)renderTarget = _renderTarget;
	}
	void SetTextureParam(Texture* texture, int32 index)
	{
		if (texture == 0) return;
		if (index < TEXTURED_MAX)
			it[index] = texture;
	}
	void SetZBuffr(ZBuffer* _zBuffer)
	{
		if (_zBuffer == 0) return;
		zBuffer = _zBuffer;
	}

	void SetRenderState(uint32* state)
	{
		if (state == 0)return;
		renderState = state;
	}

	void EnableTextureMulColor(bool _textureMulColor)
	{
		textureMulColor = _textureMulColor;
	}
 
	virtual void DrawTriangle(
		const Vertex* v0,
		const Vertex* v1,
		const Vertex* v2) = 0;
	virtual void DrawLine(
		const Vector2& v0,
		const Vector2& v1,
		const ColorU32& color);

	virtual void DrawLine(
		const Vertex* v0,
		const Vertex* v1,
		const ColorU32& color);

	virtual void DrawTexture();

protected:
	Trapezioid* tris[2];

	Texture* renderTarget;           // 帧缓冲区 = 目标颜色 用做渲染目标的纹理
	Texture* it[TEXTURED_MAX];     // 物体的纹理最多4个纹理混合
	ZBuffer* zBuffer;
	uint32* renderState;
	bool textureMulColor;  // 光栅化过程中是否与顶点颜色相乘

	virtual void FillTopFlatTriangle(
		const Vertex* v0,
		const Vertex* v1,
		const Vertex* v2,
		const int32 xMin,
		const int32 xMax,
		const int32 yMin,
		const int32 yMax) = 0;  //平顶三角形

	virtual void FillBottomFlatTriangle(
		const Vertex* v0,
		const Vertex* v1,
		const Vertex* v2,
		const int32 xMin,
		const int32 xMax,
		const int32 yMin,
		const int32 yMax) = 0;   //平底三角形

	virtual void VertexInterp(
		Vertex_Tri* rgb,
		const Vertex_Tri* rgb1,
		const Vertex_Tri* rgb2,
		float32 t) = 0;

	virtual void VertexDivision(
		Vertex_Tri* rgb,
		const Vertex_Tri* rgb1,
		const Vertex_Tri* rgb2,
		float32 w) = 0;

	virtual void VertexAdd(Vertex_Tri* a, Vertex_Tri* b) = 0;

private:

	bool CS_LineClip(
		const Vector2& v0,
		const Vector2& v1,
		Vector2& p0,
		Vector2& p1,
		const Rect& rect);

	int GetClipMode(const Vector2& v, const Rect& rect);

	void Draw_Bresenham_Line16_Decal(
		Texture* image, uint16 color,
		Vector2& startPos, Vector2& endPos);

	void Draw_Bresenham_Line24_Decal(
		Texture* image, uint32 color,
		Vector2& startPos, Vector2& endPos);

	void Draw_Bresenham_Line32_Decal(
		Texture* image, uint32 color,
		Vector2& startPos, Vector2& endPos);
};

inline IShader::IShader()
{
	for (int32 i = 0;i < 2;i++)
	{
		tris[i] = new Trapezioid();
	}

	for (int32 i = 0;i < TEXTURED_MAX;i++)
	{
		it[i] = 0;
	}
	renderTarget = 0;
	zBuffer = 0;
}
inline IShader::~IShader() 
{
	renderTarget = 0;
	zBuffer = 0;
	renderState = 0;
	//Debug::LogInfo("移除作色器");
	//SafeDelete(renderTarget);
	//for (int32 i = 0;i < TEXTURED_MAX;i++)
	//{
	//	SafeDelete(it[i]);
	//}
	//SafeDelete(zBuffer);
	//SafeDeleteArray(renderState);
}

static void SwapVertexPointer(const Vertex** v0, const Vertex** v1)
{
	const Vertex* temp = 0;
	temp = *v0;
	*v0 = *v1;
	*v1 = temp;
}


#endif

