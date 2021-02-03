#pragma once
#ifndef __GouraudAlpha_H__
#define __GouraudAlpha_H__

#include "IShader.h"

class GouraudAlpha : public IShader
{
public:
	GouraudAlpha();
	~GouraudAlpha();

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

inline GouraudAlpha::GouraudAlpha() {}
inline GouraudAlpha::~GouraudAlpha() {}


void GouraudAlpha::DrawTriangle(
	const Vertex* v0,
	const Vertex* v1,
	const Vertex* v2)
{
	int32 triType  = TRI_TYPE_NONE;
	Rect* rect = renderTarget->GetRect();

	int32 xMin = int32(rect->x);
	int32 yMin = int32(rect->y);
	int32 xMax = int32(rect->GetXMax());
	int32 yMax = int32(rect->GetYMax());

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
		if (v0->position.y == v1->position.y)
		{
			if (v1->position.x < v0->position.y) SwapVertexPointer(&v1, &v0);
			triType = TRI_TYPE_BOTTOM;
		}
		else if (v2->position.y == v1->position.y)
		{
			if (v2->position.x > v1->position.x) SwapVertexPointer(&v2, &v1);
			triType = TRI_TYPE_TOP;
		}
		else
		{
			triType = TRI_TYPE_FLAT_GENERAL;
		}
	}

	Vertex p0;
	Vertex p1;
	Vertex p2;

	p0 = *v0;
	p1 = *v1;
	p2 = *v2;
	 
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
		float32 k = (v1->position.y - v2->position.y) / (v0->position.y - v2->position.y);
		float32 x = v2->position.x + (v0->position.x - v2->position.x) * k;

		float32 r = (v2->color.GetRed()) +
			(v0->color.GetRed() - v2->color.GetRed()) * k;

		float32 g = (v2->color.GetGreen()) +
			(v0->color.GetGreen() - v2->color.GetGreen()) * k;

		float32 b = (v2->color.GetBlue()) +
			(v0->color.GetBlue() - v2->color.GetBlue()) * k;

		float32 a = (v2->color.GetBlue()) +
			(v0->color.GetAlpha() - v2->color.GetAlpha()) * k;

		Vertex p3;
		p3.position.y = v1->position.y;
		p3.position.x = x;

		p3.color.Set(a, r, g, b);

		FillTopFlatTriangle(&p0, &p1, &p3, xMin, xMax, yMin, yMax);
		FillBottomFlatTriangle(&p1, &p3, &p2, xMin, xMax, yMin, yMax);
	}
}

// 平顶
void GouraudAlpha::FillTopFlatTriangle(
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

	int32 startY = int32(v0->position.y);
	int32 endY = int32(v2->position.y);

	if (startY < yMin) startY = yMin;
	if (endY > yMax)   endY = yMax;

	float32 r0 = v0->color.GetRed();
	float32 g0 = v0->color.GetGreen();
	float32 b0 = v0->color.GetBlue();
	float32 a0 = v0->color.GetAlpha();

	float32 r1 = v1->color.GetRed();
	float32 g1 = v1->color.GetGreen();
	float32 b1 = v1->color.GetBlue();
	float32 a1 = v1->color.GetAlpha();

	float32 r2 = v2->color.GetRed();
	float32 g2 = v2->color.GetGreen();
	float32 b2 = v2->color.GetBlue();
	float32 a2 = v2->color.GetAlpha();

	Vertex_Tri vt0;
	vt0.r = r0;
	vt0.g = g0;
	vt0.b = b0;
	vt0.a = a0;
	vt0.x = v0->position.x;
	vt0.zValue = v0->zValue;

	Vertex_Tri vt1;
	vt1.r = r1;
	vt1.g = g1;
	vt1.b = b1;
	vt1.a = a1;
	vt1.x = v1->position.x;
	vt1.zValue = v1->zValue;

	Vertex_Tri vt2;
	vt2.r = r2;
	vt2.g = g2;
	vt2.b = b2;
	vt2.a = a2;
	vt2.x = v2->position.x;
	vt2.zValue = v2->zValue;

	for (int32 y = startY; y < endY; ++y)
	{
		// 计算x1,x2
		// 像素中心线到v_3.y的距离
		float32 t = (y - startY) / yLength;

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

		for (int32 x = x1;x < x2;x++)
		{
			if (x > 0)
			{
				uint8 r = uint8(tri.r * 255.0F);
				uint8 g = uint8(tri.g * 255.0F);
				uint8 b = uint8(tri.b * 255.0F);
				uint8 a = uint8(tri.a * 255.0F);

				ColorU32 color =
					((a << 24) & 0xFF000000) |
					((r << 16) & 0x00FF0000) |
					((g << 8) & 0x0000FF00) |
					(b & 0x000000FF);

				renderTarget->SetPixel(uint16(x), uint16(y), color, 1.0F);
			}

			VertexAdd(&tri, &tris[0]->step);
		}
	}

}

void GouraudAlpha::FillBottomFlatTriangle(
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

	int32 startY = int32(v0->position.y);
	int32 endY = int32(v2->position.y);

	if (startY < yMin) startY = yMin;
	if (endY > yMax)   endY = yMax;

	float32 r0 = v0->color.GetRed();
	float32 g0 = v0->color.GetGreen();
	float32 b0 = v0->color.GetBlue();
	float32 a0 = v0->color.GetAlpha();

	float32 r1 = v1->color.GetRed();
	float32 g1 = v1->color.GetGreen();
	float32 b1 = v1->color.GetBlue();
	float32 a1 = v1->color.GetAlpha();

	float32 r2 = v2->color.GetRed();
	float32 g2 = v2->color.GetGreen();
	float32 b2 = v2->color.GetBlue();
	float32 a2 = v2->color.GetAlpha();

	Vertex_Tri vt0;
	vt0.r = r0;
	vt0.g = g0;
	vt0.b = b0;
	vt0.a = a0;
	vt0.x = v0->position.x;
	vt0.zValue = v0->zValue;

	Vertex_Tri vt1;
	vt1.r = r1;
	vt1.g = g1;
	vt1.b = b1;
	vt1.a = a1;
	vt1.x = v1->position.x;
	vt1.zValue = v1->zValue;

	Vertex_Tri vt2;
	vt2.r = r2;
	vt2.g = g2;
	vt2.b = b2;
	vt2.a = a2;
	vt2.x = v2->position.x;
	vt2.zValue = v2->zValue;

	for (int32 y = startY;y < endY;y++)
	{
		// 计算x1,x2
		// 像素中心线到v_3.y的距离
		float32 t = (y - startY) / yLength;

		VertexInterp(
			&tris[1]->left, &vt0, &vt2,
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

		for (int32 x = x1;x < x2;x++)
		{
			if (x > 0)
			{
				uint8 r = uint8(tri.r * 255.0F);
				uint8 g = uint8(tri.g * 255.0F);
				uint8 b = uint8(tri.b * 255.0F);
				uint8 a = uint8(tri.a * 255.0F);

				ColorU32 color =
					((a << 24) & 0xFF000000) |
					((r << 16) & 0x00FF0000) |
					((g << 8) & 0x0000FF00) |
					(b & 0x000000FF);

				renderTarget->SetPixel(uint16(x), uint16(y), color, 1.0F);
			}

			VertexAdd(&tri, &tris[1]->step);
		}
	}
}


void GouraudAlpha::VertexInterp(
	Vertex_Tri* rgb,
	const Vertex_Tri* rgb1,
	const Vertex_Tri* rgb2,
	float32 t)
{
	rgb->x = Lerp(rgb1->x, rgb2->x, t);
	if (renderState[Z_BUFFER])
		rgb->zValue = Lerp(rgb1->zValue, rgb2->zValue, t);

	rgb->r = Lerp(rgb1->r, rgb2->r, t);
	rgb->g = Lerp(rgb1->g, rgb2->g, t);
	rgb->b = Lerp(rgb1->b, rgb2->b, t);
	rgb->a = Lerp(rgb1->a, rgb2->a, t);
}

void GouraudAlpha::VertexDivision(
	Vertex_Tri* rgb,
	const Vertex_Tri* rgb1,
	const Vertex_Tri* rgb2,
	float32 w)
{
	float32 inv = 1.0f / w;
	float32 x = (rgb2->x - rgb1->x);

	if (renderState[Z_BUFFER])
	{
		float32 zValue = (rgb2->zValue - rgb1->zValue);
		rgb->zValue = zValue * inv;
	}

	float32 r = rgb2->r - rgb1->r;
	float32 g = rgb2->g - rgb1->g;
	float32 b = rgb2->b - rgb1->b;
	float32 a = rgb2->a - rgb1->a;

	rgb->x = x * inv;
	rgb->r = r * inv;
	rgb->g = g * inv;
	rgb->b = b * inv;
	rgb->a = a * inv;
}

void GouraudAlpha::VertexAdd(Vertex_Tri* a, Vertex_Tri* b)
{
	if (renderState[Z_BUFFER])
		a->zValue += b->zValue;

	a->x += b->x;
	a->r += b->r;
	a->g += b->g;
	a->b += b->b;
	a->a += b->a;
}


#endif
