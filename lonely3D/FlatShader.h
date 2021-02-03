#pragma once
#ifndef __FlatShader_H__
#define __FlatShader_H__

#include "IShader.h"

class FlatShader : public IShader
{
public:
	FlatShader();
	~FlatShader();

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
		const int32 yMax);  //ƽ��������

	virtual void FillBottomFlatTriangle(
		const Vertex* v0,
		const Vertex* v1,
		const Vertex* v2,
		const int32 xMin,
		const int32 xMax,
		const int32 yMin,
		const int32 yMax);   //ƽ��������

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

inline FlatShader::FlatShader() {}
inline FlatShader::~FlatShader() {}

// ����������
void FlatShader::DrawTriangle(
	const Vertex* v0,
	const Vertex* v1,
	const Vertex* v2)
{
	int32 triType = TRI_TYPE_NONE;

	// ��������
	// ���������ͣ�ƽ����ƽ�ף�����(ƽ�׺�ƽ���Ļ�ϣ���Ҫ��ֳ�ƽ�׺�ƽ��)
	// �ж�����������
	Rect* rect = renderTarget->GetRect();
	int32 xMin = int32(rect->x);         int32 yMin = int32(rect->y);
	int32 xMax = int32(rect->GetXMax()); int32 yMax = int32(rect->GetYMax());

	// �򵥲ü�
	{
		if ((v0->position.y < yMin && v1->position.y < yMin && v2->position.y < yMin) ||
			(v0->position.y > yMax && v1->position.y > yMax && v2->position.y > yMax) ||
			(v0->position.x < xMin && v1->position.x < xMin && v2->position.x < xMin) ||
			(v0->position.x > xMax && v1->position.x > xMax && v2->position.x > xMax))
		{
			return;
		}
	}

	// ��������� ��y
	{
		if (v0->position.y > v1->position.y) SwapVertexPointer(&v0, &v1);
		if (v0->position.y > v2->position.y) SwapVertexPointer(&v0, &v2);
		if (v1->position.y > v2->position.y) SwapVertexPointer(&v1, &v2);
	}

	// �ж�����������
	{
		if (v0->position.y == v1->position.y)  // ƽ��
		{
			if (v1->position.x < v0->position.x) SwapVertexPointer(&v1, &v0);
			triType = TRI_TYPE_BOTTOM;
		}
		else if (v2->position.y == v1->position.y)  // ƽ��
		{
			if (v1->position.x < v2->position.x) SwapVertexPointer(&v1, &v2);
			triType = TRI_TYPE_TOP;
		}
		else   //����
		{
			triType = TRI_TYPE_FLAT_GENERAL;
		}
	}

	float32 z0 = 1.0F / (v0->position.w + 0.0000001F);
	float32 z1 = 1.0F / (v1->position.w + 0.0000001F);
	float32 z2 = 1.0F / (v2->position.w + 0.0000001F);

	Vertex p0;
	Vertex p1;
	Vertex p2;

	p0 = *v0;
	p1 = *v1;
	p2 = *v2;

	p0.zValue = z0;
	p1.zValue = z1;
	p2.zValue = z2;

	if (triType == TRI_TYPE_BOTTOM)
	{
		ColorF color = Lerp(p0.color, p1.color, 0.5F);
		color = Lerp(color, p2.color, 0.5F);
		p0.color = color;

		FillBottomFlatTriangle(&p0, &p1, &p2, xMin, xMax, yMin, yMax);
	}
	else if (triType == TRI_TYPE_TOP)
	{
		ColorF color = Lerp(p0.color, p1.color, 0.5F);
		color = Lerp(color, p2.color, 0.5F);
		p0.color = color;

		FillTopFlatTriangle(&p0, &p1, &p2, xMin, xMax, yMin, yMax);
	}
	else
	{
		// �ο� https://zhuanlan.zhihu.com/p/64006712
		float32 k = (v1->position.y - v2->position.y) / (v0->position.y - v2->position.y);
		float32 x = v2->position.x + (v0->position.x - v2->position.x) * k;

		float32 r = (v2->color.GetRed()) +
			(v0->color.GetRed() - v2->color.GetRed()) * k;

		float32 g = (v2->color.GetGreen()) +
			(v0->color.GetGreen() - v2->color.GetGreen()) * k;

		float32 b = (v2->color.GetBlue()) +
			(v0->color.GetBlue() - v2->color.GetBlue()) * k;

		float32 z = (z2)+(z0 - z2) * k;

		Vertex p3;
		p3.position.y = v1->position.y;
		p3.position.x = x;
		p3.zValue = z;
		p3.color.Set(r, g, b);

		FillTopFlatTriangle(&p0, &p1, &p3, xMin, xMax, yMin, yMax);
		FillBottomFlatTriangle(&p1, &p3, &p2, xMin, xMax, yMin, yMax);
	}

}

//ƽ��������
void FlatShader::FillTopFlatTriangle(
	const Vertex* v0,
	const Vertex* v1,
	const Vertex* v2,
	const int32 xMin,
	const int32 xMax,
	const int32 yMin,
	const int32 yMax)
{
	if (v1->position.x < v2->position.x) SwapVertexPointer(&v1, &v2);

	ColorF color0 = Lerp(v0->color, v1->color, 0.5F);
	color0 = Lerp(color0, v2->color, 0.5F);

	float32 yLength = (v2->position.y - v0->position.y);

	int32 startY = int32(v0->position.y);
	int32 endY = int32(v2->position.y);

	if (startY < yMin) startY = yMin;
	if (endY > yMax)   endY = yMax;

	float32* zbuffer = 0;

	Vertex_Tri vt0;
	vt0.x = v0->position.x;
	vt0.zValue = v0->zValue;

	Vertex_Tri vt1;
	vt1.x = v1->position.x;
	vt1.zValue = v1->zValue;

	Vertex_Tri vt2;
	vt2.x = v2->position.x;
	vt2.zValue = v2->zValue;

	for (int32 y = startY; y < endY; ++y)
	{
		zbuffer = (float32*)((uint8*)zBuffer->Lock() + y * zBuffer->GetPitch());

		// ����x1,x2
		// ���������ߵ�v_3.y�ľ���
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

		for (int32 x = x1;x <= x2;x++)
		{
			if (x > 0)
			{
				if (renderState[Z_BUFFER])
				{
					float32 zv = tri.zValue;
					if (zv > zbuffer[x])
					{
						renderTarget->SetPixel(uint16(x), uint16(y), color0);
						zbuffer[x] = zv;
					}
				}
				else
				{
					renderTarget->SetPixel(uint16(x), uint16(y), color0);
				}
			}

			VertexAdd(&tri, &tris[0]->step);
		}
	}

}

// ƽ��
void FlatShader::FillBottomFlatTriangle(
	const Vertex* v0,
	const Vertex* v1,
	const Vertex* v2,
	const int32 xMin,
	const int32 xMax,
	const int32 yMin,
	const int32 yMax)
{
	if (v1->position.x < v0->position.x) SwapVertexPointer(&v1, &v0);

	ColorF color0 = Lerp(v0->color, v1->color, 0.5F);
	color0 = Lerp(color0, v2->color, 0.5F);

	float32 yLength = (v2->position.y - v0->position.y);

	int32 startY = int32(v0->position.y);
	int32 endY = int32(v2->position.y);

	if (startY < yMin) startY = yMin;
	if (endY > yMax)   endY = yMax;

	float32* zbuffer = 0;

	Vertex_Tri vt0;
	vt0.x = v0->position.x;
	vt0.zValue = v0->zValue;

	Vertex_Tri vt1;
	vt1.x = v1->position.x;
	vt1.zValue = v1->zValue;

	Vertex_Tri vt2;
	vt2.x = v2->position.x;
	vt2.zValue = v2->zValue;

	for (int32 y = startY;y < endY;y++)
	{
		zbuffer = (float32*)((uint8*)zBuffer->Lock() + y * zBuffer->GetPitch());

		// ����x1,x2
		// ���������ߵ�v_3.y�ľ���
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

		for (int32 x = x1;x <= x2;x++)
		{
			if (x > 0)
			{
				if (renderState[Z_BUFFER])
				{
					float32 zV = tri.zValue;
					if (zV > zbuffer[x])
					{
						renderTarget->SetPixel(uint16(x), uint16(y), color0);
						zbuffer[x] = zV;
					}
				}
				else
				{
					renderTarget->SetPixel(uint16(x), uint16(y), color0);
				}
			}

			VertexAdd(&tri, &tris[1]->step);

		}
	}
}


void FlatShader::VertexInterp(
	Vertex_Tri* tri,
	const Vertex_Tri* tri1,
	const Vertex_Tri* tri2,
	float32 t)
{
	tri->x = Lerp(tri1->x, tri2->x, t);
	if (renderState[Z_BUFFER])
		tri->zValue = Lerp(tri1->zValue, tri2->zValue, t);
}

void FlatShader::VertexDivision(
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

	tri->x = x * inv;
}

void FlatShader::VertexAdd(Vertex_Tri* a, Vertex_Tri* b)
{
	if (renderState[Z_BUFFER])
		a->zValue += b->zValue;

	a->x += b->x;
}

#endif
