#pragma once
#ifndef __TextureGouraud_H__
#define __TextureGouraud_H__

#include "IShader.h"

class TextureGouraud : public IShader
{
public:
	TextureGouraud();
	~TextureGouraud();

	virtual void DrawTriangle(
		const Vertex* v0,
		const Vertex* v1,
		const Vertex* v2);

protected:
	virtual void FillTopFlatTriangle(
		const Vertex* v0,
		const Vertex* v1,
		const Vertex* v2,
		const int32 xMin,
		const int32 xMax,
		const int32 yMin,
		const int32 yMax);  //平顶三角形

	virtual void FillBottomFlatTriangle(
		const Vertex* v0,
		const Vertex* v1,
		const Vertex* v2,
		const int32 xMin,
		const int32 xMax,
		const int32 yMin,
		const int32 yMax);   //平底三角形

	virtual void VertexInterp(
		Vertex_Tri* tri,
		const Vertex_Tri* tri1,
		const Vertex_Tri* tri2,
		float32 t);

	virtual void VertexDivision(
		Vertex_Tri* tri,
		const Vertex_Tri* tri1,
		const Vertex_Tri* tri2,
		float32 w);

	virtual void VertexAdd(Vertex_Tri* a, Vertex_Tri* b);

};

inline TextureGouraud::TextureGouraud() {}
inline TextureGouraud::~TextureGouraud() {}

void TextureGouraud::DrawTriangle(
	const Vertex* v0,
	const Vertex* v1,
	const Vertex* v2)
{
	int32 triType = TRI_TYPE_NONE;

	Rect* rect = renderTarget->GetRect();

	int32 xMin = int32(rect->x);         int32 yMin = int32(rect->y);
	int32 xMax = int32(rect->GetXMax()); int32 yMax = int32(rect->GetYMax());

	// 简单裁剪
	{
		if ((v0->position.x < xMin && v1->position.x < xMin && v2->position.x < xMin) ||
			(v0->position.x > xMax && v1->position.x > xMax && v2->position.x > xMax) ||
			(v0->position.y < yMin && v1->position.y < yMin && v2->position.y < yMin) ||
			(v0->position.y > yMax && v1->position.y > yMax && v2->position.y > yMax))
			return;
	}

	// 简单排序
	{
		if (v0->position.y > v1->position.y) SwapVertexPointer(&v0, &v1);
		if (v0->position.y > v2->position.y) SwapVertexPointer(&v0, &v2);
		if (v1->position.y > v2->position.y) SwapVertexPointer(&v1, &v2);
	}

	// 判断三角形类型
	{
		if (v0->position.y == v1->position.y)  // 平底
		{
			if (v1->position.x < v0->position.x) SwapVertexPointer(&v1, &v0);
			triType = TRI_TYPE_BOTTOM;
		}
		else if (v2->position.y == v1->position.y)  // 平顶
		{
			if (v1->position.x < v2->position.x) SwapVertexPointer(&v1, &v2);
			triType = TRI_TYPE_TOP;
		}
		else   //常规
		{
			triType = TRI_TYPE_FLAT_GENERAL;
		}
	}

	// z的范围应该在[0,1],如果不是则需要裁剪
	float32 z0 = 1.0F / v0->position.w;
	float32 z1 = 1.0F / v1->position.w;
	float32 z2 = 1.0F / v2->position.w;

	Vertex p0;
	Vertex p1;
	Vertex p2;

	p0 = *v0;
	p1 = *v1;
	p2 = *v2;

	p0.zValue = z0;
	p1.zValue = z1;
	p2.zValue = z2;

	p0.uv *= z0;
	p1.uv *= z1;
	p2.uv *= z2;

	if (triType == TRI_TYPE_BOTTOM)
	{
		FillBottomFlatTriangle(&p0, &p1, &p2, xMin, xMax, yMin, yMax);
	}
	else if (triType == TRI_TYPE_TOP)
	{
		FillTopFlatTriangle(&p0, &p1, &p2, xMin, xMax, yMin, yMax);
	}
	else
	{
		// 参考 https://zhuanlan.zhihu.com/p/64006712
		//float32 k = (v1->position.y - v2->position.y) / (v0->position.y - v2->position.y);
		float32 kk = (v1->position.y - v0->position.y) / 
			(v2->position.y - v0->position.y);

		float32 x = v0->position.x + (v2->position.x - v0->position.x) * kk;
		float32 u = (p0.uv.x) + (p2.uv.x - p0.uv.x) * kk;
		float32 v = (p0.uv.y) + (p2.uv.y - p0.uv.y) * kk;
		float32 z = z0 + (z2 - z0) * kk;

		float32 r = (v0->color.GetRed()) +
			(v2->color.GetRed() - v0->color.GetRed()) * kk;

		float32 g = (v0->color.GetGreen()) +
			(v2->color.GetGreen() - v0->color.GetGreen()) * kk;

		float32 b = (v0->color.GetBlue()) +
			(v2->color.GetBlue() - v0->color.GetBlue()) * kk;

		Vertex p3;
		p3.position.y = v1->position.y;
		p3.position.x = x;
		p3.zValue = z;
		p3.uv.x = u;
		p3.uv.y = v;
		p3.color.Set(r, g, b);

		FillTopFlatTriangle(&p0, &p1, &p3,    xMin, xMax, yMin, yMax);
		FillBottomFlatTriangle(&p1, &p3, &p2, xMin, xMax, yMin, yMax);
	}

}

// 平顶
void TextureGouraud::FillTopFlatTriangle(
	const Vertex* v0,
	const Vertex* v1,
	const Vertex* v2,
	const int32 xMin,
	const int32 xMax,
	const int32 yMin,
	const int32 yMax)
{
	if (v1->position.x < v2->position.x) SwapVertexPointer(&v1, &v2);

	float32 yLength = (v2->position.y - v0->position.y);

	ColorF color0(1, 1, 1, 1);

	if (textureMulColor)
	{
		color0 = Lerp(v0->color, v1->color, 0.5F);
		color0 = Lerp(color0, v2->color, 0.5F);
	}

	int32 startY = int32(v0->position.y);
	int32 endY   = int32(v2->position.y);

	int32 startYTemp = startY;

	if (startY < yMin) startY = yMin;
	if (endY   > yMax)   endY = yMax;

	float32* zbuffer = 0;

	Vertex_Tri vt0;
	vt0.u = v0->uv.x;
	vt0.v = v0->uv.y;
	vt0.x = v0->position.x;
	vt0.zValue = v0->zValue;

	Vertex_Tri vt1;
	vt1.u = v1->uv.x;
	vt1.v = v1->uv.y;
	vt1.x = v1->position.x;
	vt1.zValue = v1->zValue;

	Vertex_Tri vt2;
	vt2.u = v2->uv.x;
	vt2.v = v2->uv.y;
	vt2.x = v2->position.x;
	vt2.zValue = v2->zValue;

	for (int32 y = startY; y < endY; ++y)
	{
		zbuffer = (float32*)((uint8*)zBuffer->Lock() + y * zBuffer->GetPitch());

		// 计算x1,x2
		// 像素中心线到v_3.y的距离
		float32 t = (y - startYTemp) / yLength;

		VertexInterp(
			&tris[0]->left, &vt0, &vt2,
			t);

		VertexInterp(
			&tris[0]->right, &vt0, &vt1,
			t);

		int32 x1 = (int32)(tris[0]->left.x);
		int32 x2 = Clamp((int32)(tris[0]->right.x), 0, xMax);

		float32 width = float32(x2 - x1 + 1);
		VertexDivision(
			&tris[0]->step, &tris[0]->left, &tris[0]->right,
			width);

		Vertex_Tri tri = tris[0]->left;

		for (int32 x = x1;x <= x2;x++)
		{
			//  && (v.position.z < 0.0F && v.position.z > 1.0F)
			if (x > 0)
			{
				if (renderState[Z_BUFFER])
				{
					if (tri.zValue > zbuffer[x])
					{
						float32 uv_u = tri.u / tri.zValue;
						float32 uv_v = tri.v / tri.zValue;

						ColorF color = it[0]->GetPixelF(uv_u, uv_v);
						if (textureMulColor) color *= color0;

						renderTarget->SetPixel(uint16(x), uint16(y), color);

						zbuffer[x] = tri.zValue;
					}
				}
				else
				{
					float32 uv_u = tri.u / tri.zValue;
					float32 uv_v = tri.v / tri.zValue;

					ColorF color = it[0]->GetPixelF(uv_u, uv_v);
					if (textureMulColor) color *= color0;

					renderTarget->SetPixel(uint16(x), uint16(y), color);
				}
			}

			VertexAdd(&tri, &tris[0]->step);
		}
	}
}

//  平底
void TextureGouraud::FillBottomFlatTriangle(
	const Vertex* v0,
	const Vertex* v1,
	const Vertex* v2,
	const int32 xMin,
	const int32 xMax,
	const int32 yMin,
	const int32 yMax)
{
	if (v1->position.x < v0->position.x) SwapVertexPointer(&v1, &v0);

	float32 yLength = (v2->position.y - v0->position.y);

	ColorF color0(1, 1, 1, 1);

	if (textureMulColor)
	{
		color0 = Lerp(v0->color, v1->color, 0.5F);
		color0 = Lerp(color0, v2->color, 0.5F);
	}

	int32 startY = int32(v0->position.y);
	int32 endY   = int32(v2->position.y);

	int32 startYTemp = startY;

	if (startY < yMin) startY = yMin;
	if (endY   > yMax)   endY = yMax;

	float32* zbuffer = 0;

	Vertex_Tri vt0;
	vt0.u = v0->uv.x;
	vt0.v = v0->uv.y;
	vt0.x = v0->position.x;
	vt0.zValue = v0->zValue;

	Vertex_Tri vt1;
	vt1.u = v1->uv.x;
	vt1.v = v1->uv.y;
	vt1.x = v1->position.x;
	vt1.zValue = v1->zValue;

	Vertex_Tri vt2;
	vt2.u = v2->uv.x;
	vt2.v = v2->uv.y;
	vt2.x = v2->position.x;
	vt2.zValue = v2->zValue;

	for (int32 y = startY;y < endY;y++)
	{
		zbuffer = (float32*)((uint8*)zBuffer->Lock() + y * zBuffer->GetPitch());

		// 计算x1,x2
		// 像素中心线到v_3.y的距离
		float32 t = (y - startYTemp) / yLength;

		VertexInterp(
			&tris[1]->left,  &vt0, &vt2,
			t);
		VertexInterp(
			&tris[1]->right, &vt1, &vt2,
			t);

		int32 x1 = (int32)(tris[1]->left.x);
		int32 x2 = Clamp((int32)(tris[1]->right.x), 0, xMax);

		float32 width = float32(x2 - x1 + 1);

		VertexDivision(
			&tris[1]->step, &tris[1]->left, &tris[1]->right,
			width);

		Vertex_Tri tri = tris[1]->left;

		for (int32 x = x1;x <= x2;x++)
		{
			if (x > 0)
			{
				if (renderState[Z_BUFFER])
				{
					if (tri.zValue > zbuffer[x])
					{
						float32 uv_u = tri.u / tri.zValue;
						float32 uv_v = tri.v / tri.zValue;

						ColorF color = it[0]->GetPixelF(uv_u, uv_v);
						if (textureMulColor) color *= color0;

						renderTarget->SetPixel(uint16(x), uint16(y), color);

						zbuffer[x] = tri.zValue;
					}
				}
				else
				{
					float32 uv_u = tri.u / tri.zValue;
					float32 uv_v = tri.v / tri.zValue;

					ColorF color = it[0]->GetPixelF(uv_u, uv_v);
					if (textureMulColor) color *= color0;

					renderTarget->SetPixel(uint16(x), uint16(y), color);
				}
			}

			VertexAdd(&tri, &tris[1]->step);
		}
	}
}


void TextureGouraud::VertexInterp(
	Vertex_Tri* tri,
	const Vertex_Tri* tri1,
	const Vertex_Tri* tri2,
	float32 t)
{
	tri->x = Lerp(tri1->x, tri2->x, t);
	if (renderState[Z_BUFFER])
		tri->zValue = Lerp(tri1->zValue, tri2->zValue, t);

	tri->u = Lerp(tri1->u, tri2->u, t);
	tri->v = Lerp(tri1->v, tri2->v, t);
}

void TextureGouraud::VertexDivision(
	Vertex_Tri* tri,
	const Vertex_Tri* tri1,
	const Vertex_Tri* tri2,
	float32 w)
{
	float32 inv = 1.0f / w;

	float32 x = (tri2->x - tri1->x);

	if (renderState[Z_BUFFER])
	{
		float32 zValue = (tri2->zValue - tri1->zValue);
		tri->zValue = zValue * inv;
	}

	float32 u = tri2->u - tri1->u;
	float32 v = tri2->v - tri1->v;

	tri->x = x * inv;
	tri->u = u * inv;
	tri->v = v * inv;
}

void TextureGouraud::VertexAdd(Vertex_Tri* a, Vertex_Tri* b)
{
	if (renderState[Z_BUFFER])
		a->zValue += b->zValue;

	a->x += b->x;
	a->u += b->u;
	a->v += b->v;
}


#endif

